class String
  def is_number?
    true if Float(self) rescue false
  end

  def is_minus?
    self == "-"
  end

  def is_multiplication?
    self == "*"
  end

  def is_plus?
    self == "+"
  end

  def is_power?
    self == "^"
  end

  def is_plus_minus?
    is_plus? || is_minus?
  end

  def is_terminal?
    "*+-".include? self
  end

  def is_open_brace?
    self == "("
  end

  def is_close_brace?
    self == ")"
  end

  def is_alpha?
    self.match /^[[:alpha:]]$/
  end

  def is_whitespace?
    self == " " || self == "\t"
  end
end

class StringTokenizer
  attr_reader :expr, :i
  def initialize(str)
    @expr = str
    @i = 0
  end

  def pop
    @i += 1
    @expr[@i - 1]
  end

  def peek
    @expr[@i]
  end

  def peek_next
    @expr[@i + 1]
  end

  def can_read?
    @i < @expr.size
  end

  def can_read_alpha?
    can_read? && peek.is_alpha?
  end

  def can_read_digit?
    can_read? && peek.is_number?
  end

  def proceed!
    @i += 1
  end

  def advance!(add_pos)
    @i += add_pos
  end

  def skip_whitespace!
    @i += 1 while (can_read? && @expr[@i].is_whitespace?)
  end

  def read_numeric
    n = 0
    n = n * 10 + pop.to_i while can_read_digit?
    n
  end

  def read_name
    name = ""
    name += pop while can_read_alpha?
    name
  end

  def pos
    @i
  end
end

