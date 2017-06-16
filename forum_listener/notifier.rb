require 'ruby-growl' if Kernel.is_windows?
require 'ruby-growl/ruby_logo' if Kernel.is_windows?

class Notifier
  def notify(text)
    raise 'Abstract class'
  end
end

class LinuxNotifier
  def notify(text)
    system("notify-send -t 10000 'New matched thread' '#{text.shellescape}'")
  end
end

class MacNotifier
  def notify(text)
    system("terminal-notifier -title 'New matched thread' -message '#{text}'")
  end
end

class WindowsNotifier
  def initialize 
    @growl = Growl.new "localhost", "forum_listener", 'GNTP'
    @growl.add_notification("notification", "Forum Notifier",
                   Growl::RUBY_LOGO_PNG)
  end
  
  def notify(text)
    @growl.notify "notification", 'New matched thread', text
  end
end
