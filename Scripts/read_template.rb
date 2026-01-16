#!/usr/bin/ruby

require 'pathname'

input_file = ARGV[0]
output_file = ARGV[1]

input = File.read(input_file).gsub("\n", "\\n").gsub('"', '\\"')

path = Pathname.new(input_file)
guard = path.basename.to_s.chars.map do |char|
  if (char < 'a' || char > 'z') && (char < 'A' || char > 'Z')
    "_"
  elsif char >= 'A' && char <= 'Z'
    "_" + char
  else
    char
  end
end.join.upcase

filename_root = path.basename(path.extname).to_s + "Template"

output = "#ifndef #{guard}\n"
output += "#define #{guard}\n"
output += "#include <string>\n"
output += "inline const std::string #{filename_root} = std::string(\"#{input}\");\n"
output += "#endif\n"

File.write(output_file, output)