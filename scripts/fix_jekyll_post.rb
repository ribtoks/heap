#!/usr/bin/ruby

require 'uri'

def fix_links(str)
  str.gsub(/<a.*? href=\"(.*?)\".*?>(.*?)<\/a>/) { |match|
    link_url, link_text = $1, $2

    if link_url.start_with? "http://jamming.com.ua/" then
      link_url = URI.decode(link_url)
      link_url.sub!("http://jamming.com.ua", "{{ site.baseurl }}")
    end
    
    "[#{link_text}](#{link_url})"
  }
end

def fix_caption_images(str)
  # image with caption
  str.gsub(/\[caption.*?\]<a href=\"(.+?)\".*?><img .*?\/><\/a>(.+?)\[\/caption\]/) { |match|
    href, title = $1, $2
    img_path = href.sub("http://jamming.com.ua/wp-content/uploads/", "")
    img_title = title.strip

    "![#{img_title}]({{ site.baseurl }}/assets/images/posts/#{img_path})\n*#{img_title}*"
  }
end

def fix_images(str)
  str.gsub(/<a href="(.+?)"><img.*? src=\".+?\".*?\/><\/a>/) { |match|
    img_path = $1.sub("http://jamming.com.ua/wp-content/uploads/", "")
    img_title = ''

    "![#{img_title}]({{ site.baseurl }}/assets/images/posts/#{img_path})\n"
  }
end

def remove_gallery(str)
  str.gsub(/\[gallery ids=.*?\]/, "")
end

filepath = ARGV[0]

text = File.read(filepath)

skipping_header = 0

out = File.open(filepath, "w")

text.each_line do |line|
  next if line =~ /^type:/
  next if line =~ /^published:/
  next if line =~ /^password:/
  next if line =~ /^status:/
  next if line =~ /^parent_id:/

  # ------------------------------

  if line =~ /^meta:/ then
    skipping_header += 1
    next
  end

  if (skipping_header == 1) && (line.strip == "---") then
    skipping_header += 1
  end

  if skipping_header == 2 then
    out.puts "author: 'Taras Kushnir'"
    skipping_header += 1
  end
  
  # ------------------------------

  next if skipping_header == 1

  line.gsub!("<p>", "\n")
  line.gsub!("</p>", "")

  line.gsub!(/<br[ \/]+?>/, "\n")

  h3_match = line.match(/^<h3>(.+)<\/h3>$/)
  if h3_match != nil then
    text = h3_match.captures.first
    out.puts "\n### #{text}\n"
    next
  end

  line = remove_gallery(line)
  line = fix_caption_images(line)
  line = fix_images(line)
  line = fix_links(line)

  out.puts line
end

puts "Done"
