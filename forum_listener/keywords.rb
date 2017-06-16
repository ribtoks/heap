def get_keywords(config)
  keywords = []
  
  File.open(config.keywords_file, config.keywords_encoding).each_line do |line|
    keywords.concat line.strip.split(config.keywords_delimiter)
  end
  
  keywords
end
