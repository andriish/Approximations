module loglog_mod
  implicit none
  private

  public :: fast_log2_log2, loglog

  real(8), parameter :: ln2   = 0.693147180559945309417232121458176568075500134360255254120d0
  real(8), parameter :: lnln2 = -0.3665129205816643d0

contains

  real(8) function log2_small_uint(value) result(res)
    integer, intent(in) :: value

    if (value <= 0) then
      res = -huge(1.0d0)
      return
    end if

    res = log(real(value, kind=8)) / ln2
  end function log2_small_uint

  real(8) function fast_log2_mantissa_1_to_2_order3(value) result(res)
    real(8), intent(in) :: value
    real(8) :: dM, dM2, dM3, denom, numer, x

    dM = 0.5d0 * value
    dM2 = dM * dM
    dM3 = dM2 * dM

    denom = (((0.22977948696488379d0 * dM + 1.4961611668393175d0) * dM + 1.071708023446889d0) * dM + 0.084444549259932208d0)
    numer = (((1.1098414161667869d0 * dM + 1.4491119665946153d0) * dM - 2.0697678829202806d0) * dM - 0.48918550780729392d0)
    x = 1.0d0 / denom
    res = 1.0d0 + x * numer
  end function fast_log2_mantissa_1_to_2_order3

  real(8) function fast_log2_log2(x) result(res)
    real(8), intent(in) :: x
    real(8) :: mantissa, e, y
    integer :: exponent, integer_m

    if (.not. (x > 0.0d0)) then
      res = -huge(1.0d0)
      return
    end if

    if (x < 4.0d0) then
      res = log(log(x)) / ln2
      return
    end if

    mantissa = fraction(x)
    integer_m = exponent(x) - 1
    e = scale(mantissa, 1)
    y = fast_log2_mantissa_1_to_2_order3(e) / real(integer_m, kind=8)

    res = log2_small_uint(integer_m) + fast_log2_mantissa_1_to_2_order3(1.0d0 + y)
  end function fast_log2_log2

  real(8) function loglog(x) result(res)
    real(8), intent(in) :: x

    if (x <= 4.0d0) then
      res = log(log(x))
      return
    end if

    res = lnln2 + ln2 * fast_log2_log2(x)
  end function loglog

end module loglog_mod
