#!/usr/bin/env ruby

require 'csv'

csv = CSV.open('./vt100_colors.csv', :headers => true, :header_converters => :symbol)
csv.each do |row|
  #puts "row=#{row}"
  fg_code = row[:fg_code]
  bg_code = row[:bg_code]
  name = row[:name].
    gsub(/[^a-z]/i, '_').
    gsub(/\s+/, '_').
    upcase
  name.gsub!(/[\_]+/, '_')
  name.gsub!(/^\_+|\_+$/, '')
  #puts "fg_code = #{fg_code}"
  #puts "bg_code = #{bg_code}"
  #puts "name = #{name}"
  puts "#define VT_COLOR_#{name}_FG #{fg_code}"
  puts "#define VT_COLOR_#{name}_BG #{bg_code}"
  #puts "---"
end
