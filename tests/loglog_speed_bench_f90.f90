program loglog_speed_bench_f90
  use, intrinsic :: iso_fortran_env, only: int64, real64
  use loglog_mod, only: loglog
  implicit none

  integer(kind=int64) :: n, i, clock0, clock1, clock_rate
  real(real64), allocatable :: values(:)
  real(real64) :: x, ref, approx, std_total, fast_total, max_abs_error, frac
  real(real64) :: std_time_us, fast_time_us, speedup
  real(real64) :: std_calls_per_us, fast_calls_per_us

  n = 5000000_int64
  if (command_argument_count() > 0) then
    call parse_samples_arg(n)
  end if

  allocate(values(n))
  do i = 1, n
    frac = real(i - 1, real64) / real(max(n - 1_int64, 1_int64), real64)
    values(i) = 2.0d0 ** (2.0d0 + 12.0d0 * frac)
  end do

  std_total = 0.0d0
  fast_total = 0.0d0
  max_abs_error = 0.0d0

  call system_clock(count=clock0, count_rate=clock_rate)
  do i = 1, n
    ref = log(log(values(i)))
    std_total = std_total + ref
  end do
  call system_clock(count=clock1, count_rate=clock_rate)
  std_time_us = 1.0d6 * real(clock1 - clock0, real64) / real(clock_rate, real64)

  call system_clock(count=clock0, count_rate=clock_rate)
  do i = 1, n
    approx = loglog(values(i))
    fast_total = fast_total + approx
  end do
  call system_clock(count=clock1, count_rate=clock_rate)
  fast_time_us = 1.0d6 * real(clock1 - clock0, real64) / real(clock_rate, real64)

  do i = 1, n
    ref = log(log(values(i)))
    approx = loglog(values(i))
    max_abs_error = max(max_abs_error, abs(ref - approx))
  end do

  if (fast_time_us > 0.0d0) then
    speedup = std_time_us / fast_time_us
  else
    speedup = 0.0d0
  end if

  if (std_time_us > 0.0d0) then
    std_calls_per_us = real(n, real64) / std_time_us
  else
    std_calls_per_us = 0.0d0
  end if
  if (fast_time_us > 0.0d0) then
    fast_calls_per_us = real(n, real64) / fast_time_us
  else
    fast_calls_per_us = 0.0d0
  end if

  write(*,'(A,I0)') 'samples=', n
  write(*,'(A)') 'implementation: Fortran loglog'
  write(*,'(A,ES24.16)') 'log(log(x)) total=', std_total
  write(*,'(A,ES24.16)') 'Fortran fast loglog total=', fast_total
  write(*,'(A,ES24.16)') 'max_abs_error=', max_abs_error
  write(*,'(A,ES24.16)') 'log(log(x)) time_us=', std_time_us
  write(*,'(A,ES24.16)') 'Fortran fast loglog time_us=', fast_time_us
  write(*,'(A,ES24.16)') 'log(log(x)) calls_per_us=', std_calls_per_us
  write(*,'(A,ES24.16)') 'Fortran fast loglog calls_per_us=', fast_calls_per_us
  write(*,'(A,ES24.16)') 'speedup=', speedup

contains

  subroutine parse_samples_arg(n)
    integer(kind=int64), intent(inout) :: n
    character(len=32) :: arg
    integer :: ios

    call get_command_argument(1, arg)
    if (trim(arg) == '--help') then
      write(*,'(A)') 'Usage: loglog_speed_bench_f90 [--samples N]'
      stop 0
    end if

    if (trim(arg) == '--samples' .and. command_argument_count() >= 2) then
      call get_command_argument(2, arg)
      read(arg, *, iostat=ios) n
      if (ios /= 0) then
        write(*,'(A)') 'Invalid sample count'
        stop 1
      end if
    end if
  end subroutine parse_samples_arg

end program loglog_speed_bench_f90
