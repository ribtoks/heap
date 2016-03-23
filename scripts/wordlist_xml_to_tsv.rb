require 'nokogiri'

doc = File.open(ARGV.first) { |f| Nokogiri::XML(f) }

result_filename = ARGV.first.sub(/[.]xml$/, '.tsv')
tsv_file = File.open(result_filename, 'w')

doc.xpath('//w').each do |node|
  frequency = node['f'];
  word = node.content

  tsv_file.puts("#{frequency}	#{word}")
end

tsv_file.close

puts "Done"
