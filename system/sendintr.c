/* sendintr.c - sendintr */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  sendintr  -  Generate given interrupt on given core
 *------------------------------------------------------------------------
 */
status sendintr(
    cid32 core,     /* core on which to generate interrupt */
    int32 intr      /* interrupt number to generate */
    )
{
    // struct gic_cpuifreg* giccpuif = (struct gic_cpuifreg*)GIC_CPUIF_BASE;
	// struct gic_distreg* gicdist = (struct gic_distreg*)GIC_DIST_BASE;

    if(isbadcid(core)){
        return SYSERR;
    }

    switch(intr){
        case RESCHED:
            // TODO: send designated resched sgi to given core
            break;
        default:
            return SYSERR;
    }

    return OK;
}