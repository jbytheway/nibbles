#include "highscoreview.hpp"

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
  GET_WIDGET(TreeView, HighScoreView);
  GET_WIDGET(TextView, HighScoreMessageText);
#undef GET_WIDGET

  // Store pointers to those widgets we need to access later
  window_ = wHighScoreDialog;
  messageView_ = wHighScoreMessageText;

  // Connect signals from widgets to the UI
  uiConnections_.push_back(window_->signal_hide().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));
  uiConnections_.push_back(wHighScoreOkButton->signal_clicked().connect(
    sigc::mem_fun(this, &Impl::windowClosed)
  ));

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
  window_->hide();
}

void HighScoreView::Impl::message(const std::string& message)
{
  Glib::RefPtr<Gtk::TextBuffer> buffer = messageView_->get_buffer();
  buffer->insert(buffer->end(), message);
  Gtk::TextIter end = buffer->end();
  messageView_->scroll_to(end);
}

void HighScoreView::Impl::windowClosed()
{
  // Crazy dangerous risking reentrancy madness
  parent_->context<Machine>().process_event(events::HighScoreOk());
}

}}}

