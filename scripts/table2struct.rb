#!/usr/bin/ruby

def type_map(t)
  tm = {
    "boolean" => "bool",
    "integer" => "uint32",
    "text" => "string",
    "timestamp" => "time.Time"
  }
  return tm[t] if tm.has_key? t
  t
end

ARGF.each do |line|
  break if line.strip.empty?
  fields = line.strip.split
  field = fields[0]
  type = fields.size > 1 ? fields[1] : "string"
  type.gsub!(/[^a-z]/i, '')
  parts = field.split('_')
  name = parts.map{|p| p == "id" ? p.upcase : p.capitalize}.join
  t = type_map(type)
  puts "#{name} #{t} `db:\"#{field}\" json:\"#{field}\"`"
end
