require 'test/unit'
require_relative './util.rb'

class TcHash < Test::Unit::TestCase
  def test_hash
    out = Util.run("./build/Hash_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_hash
    out = Util.valgrind("./build/Hash_test")
    assert_equal(0, out.exitstatus)
  end
end
