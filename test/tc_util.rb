require 'test/unit'
require_relative './util.rb'

class TestUtil < Test::Unit::TestCase
  def test_util
    out = Util.run("./build/Util_test")
    assert_equal(0, out.exitstatus)
  end
end
