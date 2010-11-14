#include "playing.hpp"

#include <boost/foreach.hpp>

#include <gtkmm.h>

#include <nibbles/level.hpp>

namespace nibbles { namespace gtk { namespace ui {

class Playing::Impl {
  public:
    Impl(Playing* parent, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml);
    ~Impl();

    // Implementation of MessageSink interface
    virtual void message(std::string const&);

    // Network reactions
    void levelStart(const Message<MessageType::levelStart>&);
  private:
    // Link back to state machine
    Playing* parent_;

    // event handler connections
    sigc::connection windowHiddenConnection_;

    // controls
    Gtk::Window* window_;
    Gtk::TextView* messageView_;
    Gtk::DrawingArea* levelDisplay_;

    // game data
    boost::scoped_ptr<Level> level_;

    // convenience functions
    void redraw();

    // UI bindings
    void windowClosed();
    bool levelExposed(GdkEventExpose*);
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
  GET_WIDGET(TextView, PlayMessageText);
  GET_WIDGET(DrawingArea, LevelDisplay);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wPlayWindow;
  messageView_ = wPlayMessageText;
  levelDisplay_ = wLevelDisplay;

  // Connect signals from widgets to the UI
  windowHiddenConnection_ = window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  );
  levelDisplay_->signal_expose_event().connect(
    sigc::mem_fun(this, &Impl::levelExposed)
  );

  window_->show();
}

Playing::Impl::~Impl()
{
  // Disconnect the window hidden signal handler because we're about to hide it
  // ourselves, and we don't want to send a terminate event because of it!
  windowHiddenConnection_.disconnect();
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
  auto const& remotePlayers =
    parent_->context<Active>().remotePlayers().get<Active::SequenceTag>();
  std::vector<PlayerId> playerIds;
  BOOST_FOREACH(auto const& player, remotePlayers) {
    playerIds.push_back(player.get<id>());
  }
  level_.reset(new Level(def, playerIds));
  redraw();
}

void Playing::Impl::redraw()
{
  Glib::RefPtr<Gdk::Window> window = levelDisplay_->get_window();
  window->invalidate(false /*invalidate children*/);
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

}}}

