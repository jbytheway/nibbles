#include "playing.hpp"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <gtkmm.h>
#include <gtkglmm.h>

#include <nibbles/level.hpp>
#include <nibbles/scoretracker.hpp>

namespace nibbles { namespace gtk { namespace ui {

class Playing::Impl {
  public:
    Impl(Playing* parent, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml);
    ~Impl();

    // Implementation of MessageSink interface
    virtual void message(std::string const&);

    // Network reactions
    void levelStart(const Message<MessageType::levelStart>&);
    void newNumber(const Message<MessageType::newNumber>&);
    void tick(const Message<MessageType::tick>&);
  private:
    // Link back to state machine
    Playing* parent_;

    // event handler connections
    std::vector<sigc::connection> uiConnections_;

    // controls
    Gtk::Window* window_;
    Gtk::Label* levelName_;
    Gtk::TextView* messageView_;
    Gtk::DrawingArea* levelDisplay_;

    class PlayerScoreListColumns : public Gtk::TreeModel::ColumnRecord
    {
      public:
        PlayerScoreListColumns()
        {
          add(lives_);
          add(score_);
          // TODO: colour
          add(name_);
        }

        Gtk::TreeModelColumn<LifeCount> lives_;
        Gtk::TreeModelColumn<Score> score_;
        Gtk::TreeModelColumn<Glib::ustring> name_;
    };
    Gtk::TreeView* playerScoreList_;
    const PlayerScoreListColumns playerScoreListColumns_;
    Glib::RefPtr<Gtk::ListStore> playerScoreListStore_;

    Gtk::GL::DrawingArea glLevelDisplay_;

    // game data
    std::map<uint32_t, std::pair<PlayerId, Command>> keyBindings_;
    boost::scoped_ptr<Level> level_;
    ScoreTracker scorer_;

    // convenience functions
    Active::RemotePlayerContainer const& remotePlayers();
    std::vector<ControlledPlayer> const& localPlayers();
    void redraw();
    void refreshScores();

    // UI bindings
    void windowClosed();
    bool levelExposed(GdkEventExpose*);
    bool glLevelExposed(GdkEventExpose*);
    void keyPress(GdkEventKey*);
};

Playing::Playing(my_context context) :
  my_base(context),
  impl_(new Impl(
      this,
      this->context<Machine>().gladeXml()
    ))
{}

Playing::~Playing() = default;

void Playing::message(std::string const& message) const
{
  impl_->message(message);
}

sc::result Playing::react(
  events::Message<MessageType::levelStart> const& event
)
{
  impl_->levelStart(event.message);
  return discard_event();
}

sc::result Playing::react(
  events::Message<MessageType::newNumber> const& event
)
{
  impl_->newNumber(event.message);
  return discard_event();
}

sc::result Playing::react(
  events::Message<MessageType::tick> const& event
)
{
  impl_->tick(event.message);
  return discard_event();
}

sc::result Playing::react(
  events::Message<MessageType::gameOver> const& event
)
{
  context<Active>().highScoreReport(event.message.payload());
  return transit<HighScoreView>();
}

Playing::Impl::Impl(
  Playing* parent,
  const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml
) :
  parent_(parent),
  window_(NULL)
{
#define GET_WIDGET(type, name)                   \
  Gtk::type* w##name = NULL;                     \
  do {                                           \
    gladeXml->get_widget(#name, w##name);        \
    if (!w##name) {                              \
      throw std::runtime_error("missing "#name); \
    }                                            \
  } while (false)

  GET_WIDGET(Window, PlayWindow);
  GET_WIDGET(Label, LevelNameLabel);
  GET_WIDGET(TreeView, PlayerScoreList);
  GET_WIDGET(TextView, PlayMessageText);
  GET_WIDGET(DrawingArea, LevelDisplay);
  GET_WIDGET(VBox, PlayVBox);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wPlayWindow;
  levelName_ = wLevelNameLabel;
  playerScoreList_ = wPlayerScoreList;
  messageView_ = wPlayMessageText;
  levelDisplay_ = wLevelDisplay;

  // Arrange the GL display
  {
    auto config =
      Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DOUBLE);
    assert(config);
    auto result = glLevelDisplay_.set_gl_capability(config);
    assert(result);
  }
  glLevelDisplay_.set_size_request(500, 300);
  glLevelDisplay_.show();
  wPlayVBox->pack_end(glLevelDisplay_);

  // Attach columns
  playerScoreListStore_ = Gtk::ListStore::create(playerScoreListColumns_);
  assert(playerScoreListStore_);
  playerScoreList_->set_model(playerScoreListStore_);
  playerScoreList_->append_column("Lives", playerScoreListColumns_.lives_);
  playerScoreList_->append_column("Score", playerScoreListColumns_.score_);
  playerScoreList_->append_column("Name", playerScoreListColumns_.name_);

  // Connect signals from widgets to the UI
  uiConnections_.push_back(window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));
  uiConnections_.push_back(levelDisplay_->signal_expose_event().connect(
    sigc::mem_fun(this, &Impl::levelExposed)
  ));
  uiConnections_.push_back(glLevelDisplay_.signal_expose_event().connect(
    sigc::mem_fun(this, &Impl::glLevelExposed)
  ));
  uiConnections_.push_back(window_->signal_key_press_event().connect_notify(
    sigc::mem_fun(this, &Impl::keyPress)
  ));

  // Set up key mapping
  auto const& remoteNameIndex = remotePlayers().get<Active::NameTag>();
  BOOST_FOREACH(auto const& player, localPlayers()) {
    auto const it = remoteNameIndex.find(player.get<name>());
    if (it == remoteNameIndex.end()) continue;
    auto const controls = player.get<fields::controls>();
    for (Command comm = Command(0); comm < Command::max; ++comm) {
      keyBindings_[controls[comm]] = {it->get<id>(), comm};
    }
  }

  // Initialize scores to zero
  auto const& settings = parent_->context<Active>().settings();
  BOOST_FOREACH(auto const& player, remotePlayers()) {
    scorer_.add(player.get<id>(), settings.get<startLives>());
  }

  window_->show();
}

Playing::Impl::~Impl()
{
  // Disconnect the window hidden signal handler because we're about to hide it
  // ourselves, and we don't want to send a terminate event because of it!
  // Also all other UI stuff to prevent dangling pointers to this
  while (!uiConnections_.empty()) {
    uiConnections_.back().disconnect();
    uiConnections_.pop_back();
  }
  playerScoreList_->unset_model();
  playerScoreList_->remove_all_columns();
  window_->hide();
}

void Playing::Impl::message(const std::string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message);
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

void Playing::Impl::levelStart(const Message<MessageType::levelStart>& m)
{
  auto const& def = m.payload();
  auto fullName =
    (boost::format("Level %d: %s") % def.get<id>() % def.get<name>()).str();
  levelName_->set_text(fullName);
  auto const& playerSequence = remotePlayers().get<Active::SequenceTag>();
  std::vector<PlayerId> playerIds;
  BOOST_FOREACH(auto const& player, playerSequence) {
    playerIds.push_back(player.get<id>());
  }
  auto const& settings = parent_->context<Active>().settings();
  level_.reset(new Level(settings, def, playerIds));
  redraw();
  refreshScores();
}

void Playing::Impl::newNumber(const Message<MessageType::newNumber>& m)
{
  if (!level_) NIBBLES_FATAL("number without level");
  level_->setNumber(m.payload());
  redraw();
  refreshScores();
}

void Playing::Impl::tick(const Message<MessageType::tick>& m)
{
  if (!level_) NIBBLES_FATAL("tick without level");
  auto const& settings = parent_->context<Active>().settings();
  level_->tick(settings, scorer_, m.payload());
  redraw();
}

Active::RemotePlayerContainer const& Playing::Impl::remotePlayers()
{
  return parent_->context<Active>().remotePlayers();
}

std::vector<ControlledPlayer> const& Playing::Impl::localPlayers()
{
  return parent_->context<Active>().localPlayers();
}

void Playing::Impl::redraw()
{
  if (levelDisplay_->get_visible()) {
    auto window = levelDisplay_->get_window();
    window->invalidate(false /*invalidate children*/);
  }
  if (glLevelDisplay_.get_visible()) {
    auto window = glLevelDisplay_.get_window();
    window->invalidate(false /*invalidate children*/);
  }
}

void Playing::Impl::refreshScores()
{
  playerScoreListStore_->clear();
  auto const& playerSequence = remotePlayers().get<Active::SequenceTag>();
  BOOST_FOREACH(auto const& player, playerSequence) {
    auto it = playerScoreListStore_->append();
    auto row = *it;
    row[playerScoreListColumns_.lives_] = scorer_.getLives(player.get<id>());
    row[playerScoreListColumns_.score_] = scorer_.getScore(player.get<id>());
    row[playerScoreListColumns_.name_] = player.get<name>();
  }
}

void Playing::Impl::windowClosed()
{
  // Crazy dangerous risking reentrancy madness
  parent_->context<Machine>().process_event(events::Terminate());
}

bool Playing::Impl::levelExposed(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::Window> window = levelDisplay_->get_window();

  if(window)
  {
    double const width = levelDisplay_->get_width();
    double const height = levelDisplay_->get_height();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->set_antialias(Cairo::ANTIALIAS_NONE);

    if (event) {
      // clip to the area indicated by the expose event so that we only
      // redraw the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
      cr->clip();
    }

    if (level_) {
      // Fill with black
      cr->set_source_rgb(0, 0, 0);
      cr->paint();

      auto const& board = level_->get<fields::board>();
      auto const levelWidth = board.width();
      auto const levelHeight = board.height();
      // Rescale to game units
      double const widthAspect = width/levelWidth;
      double const heightAspect = height/levelHeight;
      double const aspect = std::min(widthAspect, heightAspect);
      cr->scale(aspect, aspect);
      // Centre the level in the display
      cr->translate(
        (widthAspect-aspect)/aspect/2*levelWidth,
        (heightAspect-aspect)/aspect/2*levelHeight
      );

      // Fill level with blue
      cr->set_source_rgb(0, 0, 1);
      cr->move_to(0, 0);
      cr->line_to(levelWidth, 0);
      cr->line_to(levelWidth, levelHeight);
      cr->line_to(0, levelHeight);
      cr->close_path();
      cr->fill();

      // Paint the walls red
      cr->set_source_rgb(1, 0, 0);
      for (size_t x=0; x<levelWidth; ++x) {
        for (size_t y=0; y<levelHeight; ++y) {
          if (board[Point(x,y)] == BoardState::wall) {
            cr->move_to(x, y);
            cr->line_to(x+1, y);
            cr->line_to(x+1, y+1);
            cr->line_to(x, y+1);
            cr->close_path();
            cr->fill();
          }
        }
      }

      // Draw the number yellow on a green background
      auto const& number = level_->get<fields::number>();
      auto const& pos = number.get<position>();
      cr->set_source_rgb(0, 0.5, 0);
      cr->move_to(
        pos.get<min>().get<fields::x>(), pos.get<min>().get<fields::y>()
      );
      cr->line_to(
        pos.get<min>().get<fields::x>(), pos.get<max>().get<fields::y>()
      );
      cr->line_to(
        pos.get<max>().get<fields::x>(), pos.get<max>().get<fields::y>()
      );
      cr->line_to(
        pos.get<max>().get<fields::x>(), pos.get<min>().get<fields::y>()
      );
      cr->close_path();
      cr->fill();
      cr->set_source_rgb(1, 1, 0);
      cr->set_font_size(pos.height()-0.2);
      cr->set_line_width(1/aspect); // Make lines one pixel wide
      cr->move_to(
        pos.get<min>().get<fields::x>()-0.1,
        pos.get<max>().get<fields::y>()-0.2
      );
      auto val = boost::lexical_cast<std::string>(number.get<value>());
      cr->text_path(val);
      cr->fill();

      // Draw the snakes
      BOOST_FOREACH(Snake const& snake, level_->get<snakes>()) {
        PlayerId playerId = snake.get<player>();
        auto color = remotePlayers().find(playerId)->get<fields::color>();
        cr->set_source_rgb(color.d_red(), color.d_green(), color.d_blue());
        BOOST_FOREACH(Point const& point, snake.get<points>()) {
          auto const x = point.get<fields::x>();
          auto const y = point.get<fields::y>();
          cr->move_to(x, y);
          cr->line_to(x+1, y);
          cr->line_to(x+1, y+1);
          cr->line_to(x, y+1);
          cr->close_path();
          cr->fill();
        }
      }
    } else {
      // Draw a red slash for no particular reason
      cr->scale(width, height);
      cr->set_line_width(0.1);
      cr->set_source_rgb(1, 0, 0);
      cr->move_to(0, 0);
      cr->line_to(1, 1);
      cr->stroke();
    }
  }

  return true;
}

// FIXME: move to cagoul
namespace {

class ScopedOrthographicProjection {
  public:
    ScopedOrthographicProjection(
      double left, double width, double top, double height
    );
    ~ScopedOrthographicProjection();
    ScopedOrthographicProjection(ScopedOrthographicProjection const&) = delete;
};

ScopedOrthographicProjection::ScopedOrthographicProjection(
    double left, double width, double top, double height
  )
{
  glMatrixMode(GL_PROJECTION);
  // save previous matrix which contains the
  // settings for the perspective projection
  glPushMatrix();
  // reset matrix
  glLoadIdentity();
  // set a 2D orthographic projection
  glOrtho(left, left+width, top, top+height, -1, 1);
  // invert the y axis, down is positive
  glScalef(1, -1, 1);
  // move the origin from the bottom left corner
  // to the upper left corner
  glTranslatef(0, -height-2*top, 0);
  glMatrixMode(GL_MODELVIEW);
}

ScopedOrthographicProjection::~ScopedOrthographicProjection()
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

}

bool Playing::Impl::glLevelExposed(GdkEventExpose* /*event*/)
{
  auto context = glLevelDisplay_.get_gl_context();
  auto drawable = glLevelDisplay_.get_gl_drawable();

  if (!drawable->gl_begin(context)) {
    NIBBLES_FATAL("couldn't begin drawing");
  }

  if (true) // FIXME
  {
    double const width = glLevelDisplay_.get_width();
    double const height = glLevelDisplay_.get_height();
#if 0
    if (event) {
      // clip to the area indicated by the expose event so that we only
      // redraw the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
      cr->clip();
    }
#endif
    if (level_) {
      glClear(GL_COLOR_BUFFER_BIT);
      auto const& board = level_->get<fields::board>();
      auto const levelWidth = board.width();
      auto const levelHeight = board.height();
      // Rescale to game units
      double const aspect = width / height;
      double const levelAspect = double(levelWidth) / levelHeight;
      std::fprintf(stderr, "aspect=%f, levelAspect=%f\n", aspect, levelAspect);
      double visibleWidth = levelWidth;
      double visibleHeight = levelHeight;
      if (aspect >= levelAspect) {
        visibleWidth *= aspect/levelAspect;
      } else {
        visibleHeight /= aspect/levelAspect;
      }
      double const left = -(visibleWidth-levelWidth)/2;
      double const top = -(visibleHeight-levelHeight)/2;
      glViewport(0, 0, width, height);
      ScopedOrthographicProjection p(left, visibleWidth, top, visibleHeight);
      glLoadIdentity();
      glBegin(GL_QUADS);
        glColor3f(0, 0, 1);
        glVertex2f(0.0, 0.0);
        glVertex2f(levelWidth, 0.0);
        glVertex2f(levelWidth, levelHeight);
        glVertex2f(0.0, levelHeight);

        // Paint the walls red
        glColor3f(1, 0, 0);
        for (size_t x=0; x<levelWidth; ++x) {
          for (size_t y=0; y<levelHeight; ++y) {
            if (board[Point(x,y)] == BoardState::wall) {
              glVertex2f(x  , y  );
              glVertex2f(x+1, y  );
              glVertex2f(x+1, y+1);
              glVertex2f(x  , y+1);
            }
          }
        }

        // Draw the number green background
        auto const& number = level_->get<fields::number>();
        auto const& pos = number.get<position>();
        glColor3f(0, 0.5, 0);
        glVertex2f(
          pos.get<min>().get<fields::x>(), pos.get<min>().get<fields::y>()
        );
        glVertex2f(
          pos.get<min>().get<fields::x>(), pos.get<max>().get<fields::y>()
        );
        glVertex2f(
          pos.get<max>().get<fields::x>(), pos.get<max>().get<fields::y>()
        );
        glVertex2f(
          pos.get<max>().get<fields::x>(), pos.get<min>().get<fields::y>()
        );

        // Draw the snakes
        BOOST_FOREACH(Snake const& snake, level_->get<snakes>()) {
          PlayerId playerId = snake.get<player>();
          auto color = remotePlayers().find(playerId)->get<fields::color>();
          glColor3f(color.d_red(), color.d_green(), color.d_blue());
          BOOST_FOREACH(Point const& point, snake.get<points>()) {
            auto const x = point.get<fields::x>();
            auto const y = point.get<fields::y>();
            glVertex2f(x, y);
            glVertex2f(x+1, y);
            glVertex2f(x+1, y+1);
            glVertex2f(x, y+1);
          }
        }
      glEnd();
#if 0
      // Write the yellow number
      cr->set_source_rgb(1, 1, 0);
      cr->set_font_size(pos.height()-0.2);
      cr->set_line_width(1/aspect); // Make lines one pixel wide
      cr->move_to(
        pos.get<min>().get<fields::x>()-0.1,
        pos.get<max>().get<fields::y>()-0.2
      );
      auto val = boost::lexical_cast<std::string>(number.get<value>());
      cr->text_path(val);
      cr->fill();
#endif
    } else {
      // TODO: do we need a "nothing there" image to aid debugging?
    }
  }

  drawable->swap_buffers();
  drawable->gl_end();

  return true;
}

void Playing::Impl::keyPress(GdkEventKey* event)
{
  auto it = keyBindings_.find(event->keyval);
  if (it == keyBindings_.end()) {
    return;
  }
  if (auto const& client =
      parent_->state_cast<Connectedness const&>().client()) {
    client->command(it->second);
  } else {
    NIBBLES_FATAL("playing but not connected");
  }
}

}}}

