/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyritht (C) 2005- by Monami software, LP.
 * 
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id$
 */

/*
 *  タイマモジュール （LPC2214用）
 *  TIMER0を使用
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>
#include <lpc2214.h>

/*
 *  タイマ割込みの割込み番号
 */
#define    INHNO_TIMER    4

#ifndef _MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef UW    CLOCK;

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *  TIMER_CLOCK はターゲットボード毎に設定
 */
#define TO_CLOCK(nume, deno)    (TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)      ((clock) * 1000 / TIMER_CLOCK)

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define MAX_CLOCK    ((CLOCK) 0xffffffff)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define GET_TOLERANCE    100
#define BEFORE_IREQ(clock) \
        ((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
    UW	        wbits;
    CLOCK	cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

    /*
     *  割込み関連の設定
     */

    /* 割り込みレベル設定(ソフトウェア) */
    define_ipm(INHNO_TIMER,-15);

    
    /* 割込みイネーブル */
    sil_wrw_mem((VP)TIMER_T0TCR, 0);
    sil_wrw_mem((VP)TIMER_T0IR, 0xff);
    sil_wrw_mem((VP)TIMER_T0TC, 0);
    sil_wrw_mem(TIMER_T0PR, 0);
    sil_wrw_mem(TIMER_T0PC, 0);
    sil_wrw_mem(TIMER_T0MCR, (MCR_INT_MR0 | MCR_RST_MR0 | MCR_STP_MR0));
    sil_wrw_mem(TIMER_T0MR0, 3675);

    /* counter enabled. */
    sil_wrw_mem((VP)TIMER_T0TCR, 1);
}


/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
    UW wbits;

    /* 割込みクリア */
    sil_wrw_mem((VP)TIMER_T0IR, 0xff);
    /* counter enabled. */
    sil_wrw_mem((VP)TIMER_T0TCR, 1);
}



/*
 *  タイマの停止処理
 */
Inline void
hw_timer_terminate()
{
    UW wbits;

    /* 割込みディセーブル */
    /* TODO */

    sil_wrw_mem((VP)TIMER_T0IR, 0xff);
}


/*
 *  タイマの現在値の読みだし
 *
 *  割込み禁止区間中で呼び出すこと．
 */
Inline CLOCK
hw_timer_get_current(void)
{
    return(TO_CLOCK(TIC_NUME, TIC_DENO) -
           sil_rew_mem((VP)TIMER_T0TC));
}


Inline BOOL
hw_timer_fetch_interrupt(void)
{
  return sil_rew_mem((VP)TIMER_T0IR) & 1;
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
