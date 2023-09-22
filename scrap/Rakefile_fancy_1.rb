class Deps

  KNOWN_EXTS = ['.h', '.c']

  attr_reader :names
  attr_reader :basenames

  def initialize(names)

    @names = []
    if names.is_a?(String) || names.is_a?(Symbol)
      names = [names.to_s]
    end
    
    names.each do |e|
      if File.exist?(e)
        @names << e
      else
        basename = File.basename(e)
        candidates = KNOWN_EXTS.map {|ext| e+ext}
        hit = false
        candidates.each do |c|
          next unless File.exist?(c)
          @names << c 
          hit = true
        end
        unless hit
          raise "Deps. Could not resolve '#{e}'"
        end
      end
    end
    @names.uniq!
    @basenames = @names.map {|e| File.basename(e, File.extname(e)) }.uniq
  end

  def _normalize_ext(x)
    return ".#{x}".gsub(/\.+/, '.')
  end

  def ext(x)
    x = _normalize_ext(x)
    Deps.new(@names.select {|e| File.extname(e) == x})
  end

  def extnot(x)
    x = _normalize_ext(x)
    Deps.new(@names.select {|e| File.extname(e) != x })
  end

  def method_missing(meth, *args, &block)
    ext(meth)
  end

  def to_s
    if @names.size == 1
      @names.first
    else
      @names.to_s
    end
  end

  def to_a
    @names
  end

  def inspect
    "Deps(#{@names})"
  end

  def root
    @basenames
  end

  def +(other)
    arr = []
    if other.is_a?(::Deps)
      arr = other.names
    elsif other.is_a?(Array)
      arr = other
    else
      raise ArgumentError.new("Dep. Invalid type for operator +")
    end
    Deps.new(@names + arr)
  end

end

