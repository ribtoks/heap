#!/usr/bin/ruby

require 'nokogiri'

directory_path = File.join(ARGV[0], File::SEPARATOR);

uris = []
dir_path = directory_path + '*.html'

p dir_path

Dir.glob(dir_path) do |html_file|
  page = Nokogiri::HTML(open(html_file))
  link_buttons = page.css('#default_content_download_button')
  unless link_buttons.empty?
    link_uri = link_buttons.first['href']
    uris << link_uri if link_uri.end_with?('dl=1')
  end
end

f = File.open("links_to_download", File::RDWR|File::CREAT)

uris.each do |uri|
  f.write(uri + "\n")
end

f.flush
f.close
