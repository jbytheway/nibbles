#include "playing.hpp"

#include <gtkmm.h>

namespace nibbles { namespace gtk { namespace ui {

class Playing::Impl {
  public:
    Impl(Playing* parent, const Glib::RefPtr<Gnome::Glade::Xml>& gladeXml);
    ~Impl();

    // Implementation of MessageSink interface
    virtual void message(std::string const&);
  private:
    // Link back to state machine
    Playing* parent_;

    // event handler connections
    sigc::connection windowHiddenConnection_;

    // controls
    Gtk::Window* window_;
    Gtk::TextView* messageView_;
    Gtk::DrawingArea* levelDisplay_;

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
    /*Gtk::Allocation = get_allocation();
    int const width = allocation.get_width();
    int const height = allocation.get_height();*/

    int const width = levelDisplay_->get_width();
    int const height = levelDisplay_->get_height();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    if (event) {
      // clip to the area indicated by the expose event so that we only
      // redraw the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
      cr->clip();
    }

    cr->scale(width, height);
    cr->set_line_width(0.1);
    cr->set_source_rgb(1, 0, 0);
    cr->move_to(0, 0);
    cr->line_to(1, 1);
    cr->stroke();
  }

  return true;
}

}}}

