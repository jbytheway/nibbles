#include "highscoreview.hpp"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <gtkmm.h>

namespace nibbles { namespace gtk { namespace ui {

class HighScoreView::Impl {
  public:
    Impl(HighScoreView* parent, const Glib::RefPtr<Gnome::Glade::Xml>&);
    ~Impl();

    // Implementation of MessageSink interface
    virtual void message(std::string const&);
  private:
    // Link back to state machine
    HighScoreView* parent_;

    // event handler connections
    std::vector<sigc::connection> uiConnections_;

    // controls
    Gtk::Window* window_;
    Gtk::TextView* messageView_;

    class HighScoreListColumns : public Gtk::TreeModel::ColumnRecord
    {
      public:
        HighScoreListColumns()
        {
          add(rank_);
          add(score_);
          add(players_);
          add(date_);
        }

        Gtk::TreeModelColumn<Rank> rank_;
        Gtk::TreeModelColumn<Score> score_;
        Gtk::TreeModelColumn<Glib::ustring> players_;
        Gtk::TreeModelColumn<Glib::ustring> date_;
    };
    Gtk::TreeView* highScoreList_;
    const HighScoreListColumns highScoreListColumns_;
    Glib::RefPtr<Gtk::ListStore> highScoreListStore_;

    // convenience functions
    void refreshScores();

    // UI bindings
    void windowClosed();
};

HighScoreView::HighScoreView(my_context context) :
  my_base(context),
  impl_(new Impl(
      this,
      this->context<Machine>().gladeXml()
    ))
{}

HighScoreView::~HighScoreView() = default;

void HighScoreView::message(std::string const& message) const
{
  impl_->message(message);
}

HighScoreView::Impl::Impl(
  HighScoreView* parent,
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

  GET_WIDGET(Dialog, HighScoreDialog);
  GET_WIDGET(Button, HighScoreOkButton);
  GET_WIDGET(TreeView, HighScoreList);
  GET_WIDGET(TextView, HighScoreMessageText);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wHighScoreDialog;
  messageView_ = wHighScoreMessageText;
  highScoreList_ = wHighScoreList;

  // Attach columns
  highScoreListStore_ = Gtk::ListStore::create(highScoreListColumns_);
  assert(highScoreListStore_);
  highScoreList_->set_model(highScoreListStore_);
  highScoreList_->append_column("Rank", highScoreListColumns_.rank_);
  highScoreList_->append_column("Score", highScoreListColumns_.score_);
  highScoreList_->append_column("Players", highScoreListColumns_.players_);
  highScoreList_->append_column("Date", highScoreListColumns_.date_);

  // Connect signals from widgets to the UI
  uiConnections_.push_back(window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));
  uiConnections_.push_back(wHighScoreOkButton->signal_clicked().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));

  refreshScores();

  window_->show();
}

HighScoreView::Impl::~Impl()
{
  // Disconnect the window hidden signal handler because we're about to hide it
  // ourselves, and we don't want to send a terminate event because of it!
  // Also all other UI stuff to prevent dangling pointers to this
  while (!uiConnections_.empty()) {
    uiConnections_.back().disconnect();
    uiConnections_.pop_back();
  }
  highScoreList_->unset_model();
  highScoreList_->remove_all_columns();
  window_->hide();
}

void HighScoreView::Impl::message(const std::string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message);
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

namespace {
  struct CompareDates {
    typedef std::pair<Rank, HighScore> Pair;
    bool operator()(Pair const& l, Pair const& r) const {
      return l.second.get<time>() < r.second.get<time>();
    }
  };
}

void HighScoreView::Impl::refreshScores()
{
  highScoreListStore_->clear();
  auto const& highScoreReport = parent_->context<Active>().highScoreReport();
  auto const& scores = highScoreReport.get<fields::scores>();
  if (scores.empty()) return;
  auto const newest =
    std::max_element(scores.begin(), scores.end(), CompareDates());
  BOOST_FOREACH(auto const& rankScorePair, scores) {
    auto it = highScoreListStore_->append();
    auto row = *it;
    row[highScoreListColumns_.rank_] = rankScorePair.first;
    HighScore const& highScore = rankScorePair.second;
    row[highScoreListColumns_.score_] = highScore.get<totalScore>();
    std::string playersDesc;
    auto const& playerScores = highScore.get<fields::playerScores>();
    BOOST_FOREACH(auto const playerScorePair, playerScores) {
      if (!playersDesc.empty()) {
        playersDesc += " ";
      }
      playersDesc += playerScorePair.first;
      if (playerScores.size() > 1) {
        playersDesc += (boost::format(" (%d)") % playerScorePair.second).str();
      }
    }
    row[highScoreListColumns_.players_] = playersDesc;
    row[highScoreListColumns_.date_] =
      boost::lexical_cast<std::string>(highScore.get<time>());
    // Highlight the new score by selecting it
    if (&rankScorePair == &*newest) {
      highScoreList_->get_selection()->select(it);
    }
  }
}

void HighScoreView::Impl::windowClosed()
{
  // Crazy dangerous risking reentrancy madness
  parent_->context<Machine>().process_event(events::HighScoreOk());
}

}}}

