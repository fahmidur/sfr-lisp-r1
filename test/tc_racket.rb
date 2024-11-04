require 'test/unit'
require_relative 'util.rb'

class TcRacket < Test::Unit::TestCase
  def test_rb_to_racket
    sample_dir = "./test/sample" #File.join(__dir__, "sample")
    Dir.open(sample_dir).each do |path|
      next if [".", ".."].member?(path)
      next unless File.extname(path) == ".lsp"
      fpath = File.join(sample_dir, path)
      puts fpath
      racket_out = Util.run("racket #{fpath} | ./normalize-output.rb")
      assert_equal(0, racket_out.exitstatus)

      lisprb_out = Util.run("./sfr-lisp.rb #{fpath} | ./normalize-output.rb")
      assert_equal(0, lisprb_out.exitstatus, "sfr-lisp.rb | ecode FAILURE in #{fpath}")
      assert_equal(racket_out.stdout, lisprb_out.stdout, "sfr-lisp.rb | stdout FAILURE in #{fpath}")

      lispcc_out = Util.run("NO_COLOR=1 ./build/sfr-lisp #{fpath} | ./normalize-output.rb")
      assert_equal(0, lispcc_out.exitstatus, "sfr-lisp.c | ecode FAILURE in #{fpath}")
      assert_equal(racket_out.stdout, lispcc_out.stdout, "sfr-lisp.c | stdout FAILURE in #{fpath}")


    end
  end
end
