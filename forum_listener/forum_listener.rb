def Kernel.is_windows?
  processor, platform, *rest = RUBY_PLATFORM.split("-")
  platform == 'mswin32' || platform == 'mingw32'
end

def Kernel.is_mac?
  RUBY_PLATFORM.downcase.include?("darwin")
end

require_relative 'forum_indexer.rb'
require_relative 'service_config.rb'
require_relative 'logger.rb'
require_relative 'keywords.rb'

class Daemon
  def service_init(config_path)
    @config = ServiceConfig.new(config_path)
    keywords = get_keywords(@config)
    @forum = ForumIndexerBase.new(@config, keywords)

    @logger = Logger.new(@config.log)
    @logger.log(nil, 'Service started')
  end

  def update_all!
    @config.update!
    new_keywords = get_keywords(@config)
    @forum.update_keywords! new_keywords
  end

  def service_main
    loop do
      print '.'
	  @logger.dot

      update_all!
      @forum.new_topics.each do |url, text|

        @logger.log(url, text)
        print "\n"
        puts @logger.build_log_entry(url, text)

        @logger.notify(url, text) unless @config.need_notify_all?
      end

      @logger.notify_all(@forum.last_new_topics) if @config.need_notify_all?
      sleep @config.sleep_value
    end

  end

  def service_stop
    exit!
  end
end

daemon = Daemon.new
daemon.service_init(ARGV.first)
daemon.service_main
