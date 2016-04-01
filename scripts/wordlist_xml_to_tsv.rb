require 'nokogiri'

def is_bad_word?(word)
  any_non_ascii = word.bytes.any? { |b| b > 127 }
  #any_tick = word.chars.any? { |c| c == '\'' }

  any_non_ascii #|| !any_tick
end

doc = File.open(ARGV.first) { |f| Nokogiri::XML(f) }

result_filename = ARGV.first.sub(/[.]xml$/, '.tsv')
tsv_file = File.open(result_filename, 'w')

doc.xpath('//w').each do |node|
  frequency = node['f'];
  word = node.content

  tsv_file.puts("#{frequency}\t#{word}") unless is_bad_word? word
end

tsv_file.close

puts "Done"
