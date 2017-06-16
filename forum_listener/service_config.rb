require 'parseconfig'

class ServiceConfig
  def initialize(config_path)
    @config_path = config_path
    update!
  end
  
  def update!
    @config = ParseConfig.new(@config_path)
  end

  def need_notify_all?
    @config.params['notify_all'] == 'true'
  end

  def forum_urls
    @config.params['urls'].values
  end

  def base_url
    @config.params['base_url']
  end

  def encoding
    @config.params['page_encoding']
  end

  def sleep_value
    @config.params['sleep_value'].to_i
  end

  def items_xpath
    @config.params['items_xpath']
  end
  
  def topic_name_xpath
    @config.params['topic_name_xpath']
  end

  def topic_date_xpath
    @config.params['topic_date_xpath']
  end

  def topic_href_xpath
    @config.params['topic_href_xpath']
  end

  def keywords_file
    @config.params['keywords_file']
  end

  def keywords_encoding
    @config.params['keywords_encoding']
  end

  def log
    @config.params['log']
  end

  def keywords_delimiter
    @config.params['keywords_delimiter']
  end
end
