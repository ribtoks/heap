#!/usr/bin/ruby

ARGF.each do |line|
  break if line.strip.empty?
  fields = line.strip.split
  field = fields[0]
  type = fields.size > 1 ? fields[1] : "string"
  type.gsub!(/[^a-z]/i, '')
  parts = field.split('_')
  name = parts.map{|p| p == "id" ? p.upcase : p.capitalize}.join
  puts "#{name} #{type} `db:\"#{field}\" json:\"#{field}\"`"
end
