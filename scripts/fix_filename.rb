#!/usr/gbin/ruby

require 'uri'

filepath = ARGV[0]
filepath.sub!(".html", ".markdown")
decoded_filepath = URI.decode(filepath)
File.rename(filepath, decoded_filepath)

