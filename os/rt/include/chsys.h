/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    chsys.h
 * @brief   System related macros and structures.
 *
 * @addtogroup system
 * @{
 */

#ifndef _CHSYS_H_
#define _CHSYS_H_

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/**
 * @name    ISRs abstraction macros
 */
/**
 * @brief   IRQ handler enter code.
 * @note    Usually IRQ handlers functions are also declared naked.
 * @note    On some architectures this macro can be empty.
 *
 * @special
 */
#define CH_IRQ_PROLOGUE()                                                   \
  PORT_IRQ_PROLOGUE();                                                      \
  _stats_increase_irq();                                                    \
  _dbg_check_enter_isr()

/**
 * @brief   IRQ handler exit code.
 * @note    Usually IRQ handlers function are also declared naked.
 * @note    This macro usually performs the final reschedule by using
 *          @p chSchIsPreemptionRequired() and @p chSchDoReschedule().
 *
 * @special
 */
#define CH_IRQ_EPILOGUE()                                                   \
  _dbg_check_leave_isr();                                                   \
  PORT_IRQ_EPILOGUE()

/**
 * @brief   Standard normal IRQ handler declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 *
 * @special
 */
#define CH_IRQ_HANDLER(id) PORT_IRQ_HANDLER(id)
/** @} */

/**
 * @name    Fast ISRs abstraction macros
 */
/**
 * @brief   Standard fast IRQ handler declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 * @note    Not all architectures support fast interrupts.
 *
 * @special
 */
#define CH_FAST_IRQ_HANDLER(id) PORT_FAST_IRQ_HANDLER(id)
/** @} */

/**
 * @name    Time conversion utilities for the realtime counter
 * @{
 */
/**
 * @brief   Seconds to realtime counter.
 * @details Converts from seconds to realtime counter cycles.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] sec       number of seconds
 * @return              The number of cycles.
 *
 * @api
 */
#define S2RTV(sec) (CH_CFG_RTC_FREQUENCY * (sec))

/**
 * @brief   Milliseconds to realtime counter.
 * @details Converts from milliseconds to realtime counter cycles.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] msec      number of milliseconds
 * @return              The number of cycles.
 *
 * @api
 */
#define MS2RTC(msec) (rtcnt_t)(((CH_CFG_RTC_FREQUENCY + 999UL) /            \
                                1000UL) * (msec))

/**
 * @brief   Microseconds to realtime counter.
 * @details Converts from microseconds to realtime counter cycles.
 * @note    The result is rounded upward to the next tick boundary.
 *
 * @param[in] usec      number of microseconds
 * @return              The number of cycles.
 *
 * @api
 */
#define US2RTC(usec) (rtcnt_t)(((CH_CFG_RTC_FREQUENCY + 999999UL) /         \
                                1000000UL) * (usec))

/**
 * @brief   Realtime counter cycles to seconds.
 * @details Converts from realtime counter cycles number to seconds.
 *
 * @param[in] n         number of cycles
 * @return              The number of seconds.
 *
 * @api
 */
#define RTC2S(n) (rtcnt_t)(CH_CFG_RTC_FREQUENCY / (freq))

/**
 * @brief   Realtime counter cycles to milliseconds.
 * @details Converts from realtime counter cycles number to milliseconds.
 *
 * @param[in] n         number of cycles
 * @return              The number of milliseconds.
 *
 * @api
 */
#define RTC2MS(n) ((n) / (CH_CFG_RTC_FREQUENCY / 1000UL))

/**
 * @brief   Realtime counter cycles to microseconds.
 * @details Converts from realtime counter cycles number to microseconds.
 *
 * @param[in] n         number of cycles
 * @return              The number of microseconds.
 *
 * @api
 */
#define RTC2US(n) ((n) / (CH_CFG_RTC_FREQUENCY / 1000000UL))
/** @} */

/**
 * @brief   Returns the current value of the system real time counter.
 * @note    This function is only available if the port layer supports the
 *          option @p CH_PORT_SUPPORTS_RT.
 *
 * @return              The value of the system realtime counter of
 *                      type rtcnt_t.
 *
 * @xclass
 */
#if CH_PORT_SUPPORTS_RT || defined(__DOXYGEN__)
#define chSysGetRealtimeCounterX() (rtcnt_t)port_rt_get_counter_value()
#endif

/**
 * @brief   Performs a context switch.
 * @note    Not a user function, it is meant to be invoked by the scheduler
 *          itself or from within the port layer.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 *
 * @special
 */
#define chSysSwitch(ntp, otp) {                                             \
                                                                            \
  _dbg_trace(otp);                                                          \
  _stats_ctxswc(ntp, otp);                                                  \
  CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp);                                     \
  port_switch(ntp, otp);                                                    \
}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void chSysInit(void);
  void chSysHalt(void);
  void chSysTimerHandlerI(void);
  syssts_t chSysGetAndLockX(void);
  void chSysRestoreLockX(syssts_t sts);
#if CH_PORT_SUPPORTS_RT
  bool chSysIsCounterWithinX(rtcnt_t cnt, rtcnt_t start, rtcnt_t end);
  void chSysPolledDelayX(rtcnt_t cycles);
#endif
#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/**
 * @brief   Raises the system interrupt priority mask to the maximum level.
 * @details All the maskable interrupt sources are disabled regardless their
 *          hardware priority.
 * @note    Do not invoke this API from within a kernel lock.
 *
 * @special
 */
static inline void chSysDisable(void) {

  port_disable();
  _dbg_check_disable();
}

/**
 * @brief   Raises the system interrupt priority mask to system level.
 * @details The interrupt sources that should not be able to preempt the kernel
 *          are disabled, interrupt sources with higher priority are still
 *          enabled.
 * @note    Do not invoke this API from within a kernel lock.
 * @note    This API is no replacement for @p chSysLock(), the @p chSysLock()
 *          could do more than just disable the interrupts.
 *
 * @special
 */
static inline void chSysSuspend(void) {

  port_suspend();
  _dbg_check_suspend();
}

/**
 * @brief   Lowers the system interrupt priority mask to user level.
 * @details All the interrupt sources are enabled.
 * @note    Do not invoke this API from within a kernel lock.
 * @note    This API is no replacement for @p chSysUnlock(), the
 *          @p chSysUnlock() could do more than just enable the interrupts.
 *
 * @special
 */
static inline void chSysEnable(void) {

  _dbg_check_enable();
  port_enable();
}

/**
 * @brief   Enters the kernel lock mode.
 *
 * @special
 */
static inline void chSysLock(void)  {

  port_lock();
  _stats_start_measure_crit_thd();
  _dbg_check_lock();
}

/**
 * @brief   Leaves the kernel lock mode.
 *
 * @special
 */
static inline void chSysUnlock(void) {

  _dbg_check_unlock();
  _stats_stop_measure_crit_thd();
  port_unlock();
}

/**
 * @brief   Enters the kernel lock mode from within an interrupt handler.
 * @note    This API may do nothing on some architectures, it is required
 *          because on ports that support preemptable interrupt handlers
 *          it is required to raise the interrupt mask to the same level of
 *          the system mutual exclusion zone.<br>
 *          It is good practice to invoke this API before invoking any I-class
 *          syscall from an interrupt handler.
 * @note    This API must be invoked exclusively from interrupt handlers.
 *
 * @special
 */
static inline void chSysLockFromISR(void) {

  port_lock_from_isr();
  _stats_start_measure_crit_isr();
  _dbg_check_lock_from_isr();
}

/**
 * @brief   Leaves the kernel lock mode from within an interrupt handler.
 *
 * @note    This API may do nothing on some architectures, it is required
 *          because on ports that support preemptable interrupt handlers
 *          it is required to raise the interrupt mask to the same level of
 *          the system mutual exclusion zone.<br>
 *          It is good practice to invoke this API after invoking any I-class
 *          syscall from an interrupt handler.
 * @note    This API must be invoked exclusively from interrupt handlers.
 *
 * @special
 */
static inline void chSysUnlockFromISR(void) {

  _dbg_check_unlock_from_isr();
  _stats_stop_measure_crit_isr();
  port_unlock_from_isr();
}

#endif /* _CHSYS_H_ */

/** @} */