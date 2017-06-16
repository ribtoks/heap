require 'shellwords'
require_relative 'notifier.rb'

class Logger
  def initialize(log_file_path)
    @log_file_path = log_file_path
    File.delete(@log_file_path) if File.exists? @log_file_path
    @dot_was_last = false

    if Kernel.is_windows?
      @notifier = WindowsNotifier.new
    elsif Kernel.is_mac?
      @notifier = MacNotifier.new
    else
      @notifier = LinuxNotifier.new
    end
  end

  def build_log_entry(url, text)
    %Q[#{url} [#{text}]]
  end

  def notify(url, text)
    @notifier.notify(text)
  end

  def log(url, text)
    open(@log_file_path, 'a') { |f|
	  f.write("\n") if @dot_was_last
      f.puts "[#{Time.now}] " + build_log_entry(url, text)
    }
	@dot_was_last = false
  end

  def dot
	open(@log_file_path, 'a') { |f|
	  f.write "."
    }
	@dot_was_last = true
  end

  def notify_all(new_topics)
    return if new_topics.size.zero?

    count = new_topics.size
    text = "New #{count} matched threads"

    if Kernel.is_windows?
      # -- Instant notification
      GNTP.notify({
                    :app_name => 'Forum Notifier',
                    :title    => "New threads",
                    :text     => text
                  })
    else
      system("notify-send -t 30000 '#{text.shellescape}'")
    end
  end
end
