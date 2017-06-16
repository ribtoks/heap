require 'nokogiri'
require 'open-uri'
require 'digest/sha1'
require 'unicode_utils/downcase'

if Kernel.is_windows?
  require 'openssl'
  OpenSSL::SSL::VERIFY_PEER = OpenSSL::SSL::VERIFY_NONE
end

class ForumIndexerBase    
  def initialize(config, keywords)
    @config = config
    @keywords = keywords
    @index = {}
  end
  
  def parse_page(page_url)
    new_items = []
    begin
      doc = Nokogiri::HTML(open(page_url), nil, @config.encoding)
      doc.xpath(@config.items_xpath).each do |item|
        parts = parse_item(item)
        new_items << parts if item_has_keywords? parts
      end
    rescue      
    end
    
    new_items
  end

  def update_keywords!(new_keywords)
    @keywords = new_keywords
  end

  def parse_item(item)
    name = item.xpath(@config.topic_name_xpath).to_s.encode('utf-8')
    date = item.xpath(@config.topic_date_xpath).to_s.strip
    href = item.xpath(@config.topic_href_xpath).to_s

    downcase_name = UnicodeUtils.downcase(name)    
    
    {:name => name, :downcase_name => downcase_name, :date => date, :href => href}
  end

  def item_has_keywords?(parsed_parts)
    return @keywords.any? { |word| parsed_parts[:downcase_name].include? UnicodeUtils.downcase(word) } unless @keywords.empty?
    true
  end

  def last_new_topics
    @last_new_topics
  end

  def new_topics
    result = []
    topics = []
    threads = []

    @config.forum_urls.each do |url|
      threads << Thread.new(url) do |thread_url|
        Thread.current['topics'] = parse_page(thread_url)
      end
    end

    threads.each do |thrd|
      thrd.join
      topics.concat thrd['topics']
    end
    
    topics.each do |topic|
      sha1 = Digest::SHA1.hexdigest(topic[:downcase_name])
      unless @index.has_key? sha1
        @index[sha1] = topic
        url = @config.base_url + topic[:href]
        result << [url, topic[:name]]
      end
    end

    @last_new_topics = result
    result
  end
  
end
