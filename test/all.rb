require 'test/unit'
cwd = File.dirname(__FILE__)
basepath = File.absolute_path(cwd + '/..')
$LOAD_PATH << basepath
Dir.glob(cwd + '/tc_*') do |path|
  require(path)
end
