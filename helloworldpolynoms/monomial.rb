class Monomial
  attr_reader :powers
  attr :coef, true

  def initialize(powers={}, coef=0.0)
    @powers = Hash.new { |hash, key| hash[key] = 0 }
    powers.each do |key, value| @powers[key] = value end
    @coef = coef
    @epsilon = 0.000001
  end

  def ==(other)
    return false if (@coef - other.coef).abs > @epsilon
    equal_power?(other)
  end
  
  def equal_power?(other)
    return false unless size == other.size
    
    other.powers.each do |key, value|
      return false unless @powers[key] == value
    end
    true
  end
  
  def size
    @powers.size
  end
  
  def *(other)
    if other.is_a?(Numeric)
      Monomial.new(@powers, @coef * other)
    elsif other.is_a?(Monomial)
      Monomial.new(@powers.merge(other.powers){|key, v1, v2| v1 + v2},
                   @coef * other.coef)
    else
      raise NotImplementedError
    end
  end
  
  def coerce(other)
    return self, other
  end
  
  def item_to_s(x, p)
    return x.to_s if p == 1
    "#{x}^#{p}"
  end

  def zero?
    @coef.abs < @epsilon
  end

  def get_hash_str
    active_powers = @powers.select{|x, p| p > 0}
    return "|" if active_powers.empty?
    h = active_powers.to_a.sort{|x, y| x[0] <=> y[0]}
    h.map{|x, p| "#{x}:#{p}"}.join("|")
  end
  
  def to_s
    return "0" if @coef.abs < @epsilon

    active_powers = @powers.select{|x, p| p > 0}
    return @coef.to_s if active_powers.empty?

    items = []
    items << @coef.to_s if @coef.abs != 1
    start = @coef == -1 ? "-" : ""
    start + items.concat(active_powers.map{|x, p| item_to_s(x, p)}).join("*")
  end  
end
