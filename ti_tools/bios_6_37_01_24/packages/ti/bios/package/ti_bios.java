/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z63
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_bios
{
    static final String VERS = "@(#) xdc-z63\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.misc");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.heaps");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.bios.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.bios", new Value.Obj("ti.bios", pkgP));
    }

    void SYS$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.SYS.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.SYS", new Value.Obj("ti.bios.SYS", po));
        pkgV.bind("SYS", vo);
        // decls 
    }

    void LOG$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.LOG.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.LOG", new Value.Obj("ti.bios.LOG", po));
        pkgV.bind("LOG", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.LOG.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.LOG$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.LOG.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.LOG$$Params", new Proto.Obj());
        om.bind("ti.bios.LOG.Params", new Proto.Str(po, true));
    }

    void MEM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.MEM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.MEM", new Value.Obj("ti.bios.MEM", po));
        pkgV.bind("MEM", vo);
        // decls 
        spo = (Proto.Obj)om.bind("ti.bios.MEM$$HeapList", new Proto.Obj());
        om.bind("ti.bios.MEM.HeapList", new Proto.Str(spo, true));
        // insts 
        Object insP = om.bind("ti.bios.MEM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.MEM$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.MEM.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.MEM$$Params", new Proto.Obj());
        om.bind("ti.bios.MEM.Params", new Proto.Str(po, true));
    }

    void TSK$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.TSK.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.TSK", new Value.Obj("ti.bios.TSK", po));
        pkgV.bind("TSK", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.TSK.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.TSK$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.TSK.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.TSK$$Params", new Proto.Obj());
        om.bind("ti.bios.TSK.Params", new Proto.Str(po, true));
    }

    void CLK$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.CLK.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.CLK", new Value.Obj("ti.bios.CLK", po));
        pkgV.bind("CLK", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.CLK.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.CLK$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.CLK.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.CLK$$Params", new Proto.Obj());
        om.bind("ti.bios.CLK.Params", new Proto.Str(po, true));
    }

    void SEM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.SEM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.SEM", new Value.Obj("ti.bios.SEM", po));
        pkgV.bind("SEM", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.SEM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.SEM$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.SEM.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.SEM$$Params", new Proto.Obj());
        om.bind("ti.bios.SEM.Params", new Proto.Str(po, true));
    }

    void SWI$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.SWI.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.SWI", new Value.Obj("ti.bios.SWI", po));
        pkgV.bind("SWI", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.SWI.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.SWI$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.SWI.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.SWI$$Params", new Proto.Obj());
        om.bind("ti.bios.SWI.Params", new Proto.Str(po, true));
    }

    void IDL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.IDL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.IDL", new Value.Obj("ti.bios.IDL", po));
        pkgV.bind("IDL", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.IDL.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.IDL$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.IDL.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.IDL$$Params", new Proto.Obj());
        om.bind("ti.bios.IDL.Params", new Proto.Str(po, true));
    }

    void MBX$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.MBX.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.MBX", new Value.Obj("ti.bios.MBX", po));
        pkgV.bind("MBX", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.MBX.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.MBX$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.MBX.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.MBX$$Params", new Proto.Obj());
        om.bind("ti.bios.MBX.Params", new Proto.Str(po, true));
    }

    void STS$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.STS.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.STS", new Value.Obj("ti.bios.STS", po));
        pkgV.bind("STS", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.STS.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.STS$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.STS.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.STS$$Params", new Proto.Obj());
        om.bind("ti.bios.STS.Params", new Proto.Str(po, true));
    }

    void QUE$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.QUE.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.QUE", new Value.Obj("ti.bios.QUE", po));
        pkgV.bind("QUE", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.QUE.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.QUE$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.QUE.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.QUE$$Params", new Proto.Obj());
        om.bind("ti.bios.QUE.Params", new Proto.Str(po, true));
    }

    void PRD$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.PRD.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.PRD", new Value.Obj("ti.bios.PRD", po));
        pkgV.bind("PRD", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.PRD.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.PRD$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.PRD.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.PRD$$Params", new Proto.Obj());
        om.bind("ti.bios.PRD.Params", new Proto.Str(po, true));
    }

    void HWI$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.HWI.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.HWI", new Value.Obj("ti.bios.HWI", po));
        pkgV.bind("HWI", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.HWI.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.HWI$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.HWI.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.HWI$$Params", new Proto.Obj());
        om.bind("ti.bios.HWI.Params", new Proto.Str(po, true));
    }

    void PIE$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.PIE.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.PIE", new Value.Obj("ti.bios.PIE", po));
        pkgV.bind("PIE", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.PIE.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.PIE$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.PIE.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.PIE$$Params", new Proto.Obj());
        om.bind("ti.bios.PIE.Params", new Proto.Str(po, true));
    }

    void BUF$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.BUF.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.BUF", new Value.Obj("ti.bios.BUF", po));
        pkgV.bind("BUF", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.BUF.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.BUF$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.BUF.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.BUF$$Params", new Proto.Obj());
        om.bind("ti.bios.BUF.Params", new Proto.Str(po, true));
    }

    void LCK$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.LCK.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.LCK", new Value.Obj("ti.bios.LCK", po));
        pkgV.bind("LCK", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.LCK.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.LCK$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.LCK.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.LCK$$Params", new Proto.Obj());
        om.bind("ti.bios.LCK.Params", new Proto.Str(po, true));
    }

    void HOOK$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.HOOK.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.HOOK", new Value.Obj("ti.bios.HOOK", po));
        pkgV.bind("HOOK", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.HOOK.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.HOOK$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.HOOK.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.HOOK$$Params", new Proto.Obj());
        om.bind("ti.bios.HOOK.Params", new Proto.Str(po, true));
    }

    void GBL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.GBL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.GBL", new Value.Obj("ti.bios.GBL", po));
        pkgV.bind("GBL", vo);
        // decls 
    }

    void HST$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.HST.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.HST", new Value.Obj("ti.bios.HST", po));
        pkgV.bind("HST", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.HST.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.HST$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.HST.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.HST$$Params", new Proto.Obj());
        om.bind("ti.bios.HST.Params", new Proto.Str(po, true));
    }

    void PIP$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.PIP.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.PIP", new Value.Obj("ti.bios.PIP", po));
        pkgV.bind("PIP", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.PIP.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.PIP$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.PIP.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.PIP$$Params", new Proto.Obj());
        om.bind("ti.bios.PIP.Params", new Proto.Str(po, true));
    }

    void RTDX$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.RTDX.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.RTDX", new Value.Obj("ti.bios.RTDX", po));
        pkgV.bind("RTDX", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.RTDX.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.RTDX$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.RTDX.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.RTDX$$Params", new Proto.Obj());
        om.bind("ti.bios.RTDX.Params", new Proto.Str(po, true));
    }

    void ECM$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.ECM.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.ECM", new Value.Obj("ti.bios.ECM", po));
        pkgV.bind("ECM", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.ECM.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.ECM$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.ECM.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.ECM$$Params", new Proto.Obj());
        om.bind("ti.bios.ECM.Params", new Proto.Str(po, true));
    }

    void DEV$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.DEV.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.DEV", new Value.Obj("ti.bios.DEV", po));
        pkgV.bind("DEV", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.DEV.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.DEV$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.DEV.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.DEV$$Params", new Proto.Obj());
        om.bind("ti.bios.DEV.Params", new Proto.Str(po, true));
    }

    void DGN$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.DGN.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.DGN", new Value.Obj("ti.bios.DGN", po));
        pkgV.bind("DGN", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.DGN.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.DGN$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.DGN.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.DGN$$Params", new Proto.Obj());
        om.bind("ti.bios.DGN.Params", new Proto.Str(po, true));
    }

    void DHL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.DHL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.DHL", new Value.Obj("ti.bios.DHL", po));
        pkgV.bind("DHL", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.DHL.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.DHL$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.DHL.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.DHL$$Params", new Proto.Obj());
        om.bind("ti.bios.DHL.Params", new Proto.Str(po, true));
    }

    void DPI$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.DPI.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.DPI", new Value.Obj("ti.bios.DPI", po));
        pkgV.bind("DPI", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.DPI.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.DPI$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.DPI.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.DPI$$Params", new Proto.Obj());
        om.bind("ti.bios.DPI.Params", new Proto.Str(po, true));
    }

    void DIO$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.DIO.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.DIO", new Value.Obj("ti.bios.DIO", po));
        pkgV.bind("DIO", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.DIO.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.DIO$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.DIO.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.DIO$$Params", new Proto.Obj());
        om.bind("ti.bios.DIO.Params", new Proto.Str(po, true));
    }

    void GIO$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.GIO.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.GIO", new Value.Obj("ti.bios.GIO", po));
        pkgV.bind("GIO", vo);
        // decls 
    }

    void SIO$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.SIO.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.SIO", new Value.Obj("ti.bios.SIO", po));
        pkgV.bind("SIO", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.SIO.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.SIO$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.SIO.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.SIO$$Params", new Proto.Obj());
        om.bind("ti.bios.SIO.Params", new Proto.Str(po, true));
    }

    void UDEV$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.UDEV.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.UDEV", new Value.Obj("ti.bios.UDEV", po));
        pkgV.bind("UDEV", vo);
        // decls 
        // insts 
        Object insP = om.bind("ti.bios.UDEV.Instance", new Proto.Obj());
        po = (Proto.Obj)om.bind("ti.bios.UDEV$$Object", new Proto.Obj());
        Object objP = om.bind("ti.bios.UDEV.Object", new Proto.Str(po, true));
        po = (Proto.Obj)om.bind("ti.bios.UDEV$$Params", new Proto.Obj());
        om.bind("ti.bios.UDEV.Params", new Proto.Str(po, true));
    }

    void MSGQ$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.MSGQ.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.MSGQ", new Value.Obj("ti.bios.MSGQ", po));
        pkgV.bind("MSGQ", vo);
        // decls 
    }

    void POOL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.POOL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.POOL", new Value.Obj("ti.bios.POOL", po));
        pkgV.bind("POOL", vo);
        // decls 
    }

    void STATICPOOL$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.bios.STATICPOOL.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.bios.STATICPOOL", new Value.Obj("ti.bios.STATICPOOL", po));
        pkgV.bind("STATICPOOL", vo);
        // decls 
    }

    void SYS$$CONSTS()
    {
        // module SYS
    }

    void LOG$$CONSTS()
    {
        // module LOG
    }

    void MEM$$CONSTS()
    {
        // module MEM
    }

    void TSK$$CONSTS()
    {
        // module TSK
    }

    void CLK$$CONSTS()
    {
        // module CLK
    }

    void SEM$$CONSTS()
    {
        // module SEM
    }

    void SWI$$CONSTS()
    {
        // module SWI
    }

    void IDL$$CONSTS()
    {
        // module IDL
    }

    void MBX$$CONSTS()
    {
        // module MBX
    }

    void STS$$CONSTS()
    {
        // module STS
    }

    void QUE$$CONSTS()
    {
        // module QUE
    }

    void PRD$$CONSTS()
    {
        // module PRD
    }

    void HWI$$CONSTS()
    {
        // module HWI
    }

    void PIE$$CONSTS()
    {
        // module PIE
    }

    void BUF$$CONSTS()
    {
        // module BUF
    }

    void LCK$$CONSTS()
    {
        // module LCK
    }

    void HOOK$$CONSTS()
    {
        // module HOOK
    }

    void GBL$$CONSTS()
    {
        // module GBL
    }

    void HST$$CONSTS()
    {
        // module HST
    }

    void PIP$$CONSTS()
    {
        // module PIP
    }

    void RTDX$$CONSTS()
    {
        // module RTDX
    }

    void ECM$$CONSTS()
    {
        // module ECM
    }

    void DEV$$CONSTS()
    {
        // module DEV
    }

    void DGN$$CONSTS()
    {
        // module DGN
    }

    void DHL$$CONSTS()
    {
        // module DHL
    }

    void DPI$$CONSTS()
    {
        // module DPI
    }

    void DIO$$CONSTS()
    {
        // module DIO
    }

    void GIO$$CONSTS()
    {
        // module GIO
    }

    void SIO$$CONSTS()
    {
        // module SIO
    }

    void UDEV$$CONSTS()
    {
        // module UDEV
    }

    void MSGQ$$CONSTS()
    {
        // module MSGQ
    }

    void POOL$$CONSTS()
    {
        // module POOL
    }

    void STATICPOOL$$CONSTS()
    {
        // module STATICPOOL
    }

    void SYS$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void LOG$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.LOG$$create", new Proto.Fxn(om.findStrict("ti.bios.LOG.Module", "ti.bios"), om.findStrict("ti.bios.LOG.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.LOG.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$LOG$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.LOG'];\n");
            sb.append("var __inst = xdc.om['ti.bios.LOG.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.format = __mod.PARAMS.format;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.LOG$$construct", new Proto.Fxn(om.findStrict("ti.bios.LOG.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.LOG$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.LOG.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$LOG$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.LOG'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.format = __mod.PARAMS.format;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void MEM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$create", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), om.findStrict("ti.bios.MEM.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.MEM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$MEM$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.MEM'];\n");
            sb.append("var __inst = xdc.om['ti.bios.MEM.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.heapId = __mod.PARAMS.heapId;\n");
            sb.append("__inst.heapMemInst = __mod.PARAMS.heapMemInst;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$construct", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.MEM$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.MEM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$MEM$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.MEM'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.heapId = __mod.PARAMS.heapId;\n");
            sb.append("__inst.heapMemInst = __mod.PARAMS.heapMemInst;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void TSK$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.TSK$$create", new Proto.Fxn(om.findStrict("ti.bios.TSK.Module", "ti.bios"), om.findStrict("ti.bios.TSK.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.TSK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$TSK$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.TSK'];\n");
            sb.append("var __inst = xdc.om['ti.bios.TSK.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.manualStack = __mod.PARAMS.manualStack;\n");
            sb.append("__inst.priority = __mod.PARAMS.priority;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.TSK$$construct", new Proto.Fxn(om.findStrict("ti.bios.TSK.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.TSK$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.TSK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$TSK$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.TSK'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.manualStack = __mod.PARAMS.manualStack;\n");
            sb.append("__inst.priority = __mod.PARAMS.priority;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void CLK$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.CLK$$create", new Proto.Fxn(om.findStrict("ti.bios.CLK.Module", "ti.bios"), om.findStrict("ti.bios.CLK.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.CLK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$CLK$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.CLK'];\n");
            sb.append("var __inst = xdc.om['ti.bios.CLK.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.CLK$$construct", new Proto.Fxn(om.findStrict("ti.bios.CLK.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.CLK$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.CLK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$CLK$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.CLK'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void SEM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.SEM$$create", new Proto.Fxn(om.findStrict("ti.bios.SEM.Module", "ti.bios"), om.findStrict("ti.bios.SEM.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.SEM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SEM$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SEM'];\n");
            sb.append("var __inst = xdc.om['ti.bios.SEM.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.SEM$$construct", new Proto.Fxn(om.findStrict("ti.bios.SEM.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.SEM$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.SEM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SEM$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SEM'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void SWI$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.SWI$$create", new Proto.Fxn(om.findStrict("ti.bios.SWI.Module", "ti.bios"), om.findStrict("ti.bios.SWI.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.SWI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SWI$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SWI'];\n");
            sb.append("var __inst = xdc.om['ti.bios.SWI.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.SWI$$construct", new Proto.Fxn(om.findStrict("ti.bios.SWI.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.SWI$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.SWI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SWI$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SWI'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void IDL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.IDL$$create", new Proto.Fxn(om.findStrict("ti.bios.IDL.Module", "ti.bios"), om.findStrict("ti.bios.IDL.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.IDL.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$IDL$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.IDL'];\n");
            sb.append("var __inst = xdc.om['ti.bios.IDL.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.calibration = __mod.PARAMS.calibration;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.IDL$$construct", new Proto.Fxn(om.findStrict("ti.bios.IDL.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.IDL$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.IDL.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$IDL$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.IDL'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.calibration = __mod.PARAMS.calibration;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void MBX$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.MBX$$create", new Proto.Fxn(om.findStrict("ti.bios.MBX.Module", "ti.bios"), om.findStrict("ti.bios.MBX.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.MBX.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$MBX$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.MBX'];\n");
            sb.append("var __inst = xdc.om['ti.bios.MBX.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.MBX$$construct", new Proto.Fxn(om.findStrict("ti.bios.MBX.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.MBX$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.MBX.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$MBX$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.MBX'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void STS$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.STS$$create", new Proto.Fxn(om.findStrict("ti.bios.STS.Module", "ti.bios"), om.findStrict("ti.bios.STS.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.STS.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$STS$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.STS'];\n");
            sb.append("var __inst = xdc.om['ti.bios.STS.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.STS$$construct", new Proto.Fxn(om.findStrict("ti.bios.STS.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.STS$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.STS.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$STS$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.STS'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void QUE$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.QUE$$create", new Proto.Fxn(om.findStrict("ti.bios.QUE.Module", "ti.bios"), om.findStrict("ti.bios.QUE.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.QUE.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$QUE$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.QUE'];\n");
            sb.append("var __inst = xdc.om['ti.bios.QUE.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.QUE$$construct", new Proto.Fxn(om.findStrict("ti.bios.QUE.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.QUE$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.QUE.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$QUE$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.QUE'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void PRD$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.PRD$$create", new Proto.Fxn(om.findStrict("ti.bios.PRD.Module", "ti.bios"), om.findStrict("ti.bios.PRD.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.PRD.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PRD$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PRD'];\n");
            sb.append("var __inst = xdc.om['ti.bios.PRD.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.arg1 = __mod.PARAMS.arg1;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.PRD$$construct", new Proto.Fxn(om.findStrict("ti.bios.PRD.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.PRD$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.PRD.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PRD$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PRD'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.fxn = __mod.PARAMS.fxn;\n");
            sb.append("__inst.arg1 = __mod.PARAMS.arg1;\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void HWI$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.HWI$$create", new Proto.Fxn(om.findStrict("ti.bios.HWI.Module", "ti.bios"), om.findStrict("ti.bios.HWI.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.HWI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HWI$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HWI'];\n");
            sb.append("var __inst = xdc.om['ti.bios.HWI.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.interruptSelectNumber = __mod.PARAMS.interruptSelectNumber;\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("__inst.addr = __mod.PARAMS.addr;\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.operation = __mod.PARAMS.operation;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.HWI$$construct", new Proto.Fxn(om.findStrict("ti.bios.HWI.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.HWI$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.HWI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HWI$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HWI'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.interruptSelectNumber = __mod.PARAMS.interruptSelectNumber;\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("__inst.addr = __mod.PARAMS.addr;\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.operation = __mod.PARAMS.operation;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void PIE$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.PIE$$create", new Proto.Fxn(om.findStrict("ti.bios.PIE.Module", "ti.bios"), om.findStrict("ti.bios.PIE.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.PIE.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PIE$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PIE'];\n");
            sb.append("var __inst = xdc.om['ti.bios.PIE.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("__inst.addr = __mod.PARAMS.addr;\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.operation = __mod.PARAMS.operation;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.PIE$$construct", new Proto.Fxn(om.findStrict("ti.bios.PIE.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.PIE$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.PIE.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PIE$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PIE'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("__inst.addr = __mod.PARAMS.addr;\n");
            sb.append("__inst.dataType = __mod.PARAMS.dataType;\n");
            sb.append("__inst.operation = __mod.PARAMS.operation;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void BUF$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.BUF$$create", new Proto.Fxn(om.findStrict("ti.bios.BUF.Module", "ti.bios"), om.findStrict("ti.bios.BUF.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.BUF.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$BUF$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.BUF'];\n");
            sb.append("var __inst = xdc.om['ti.bios.BUF.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.BUF$$construct", new Proto.Fxn(om.findStrict("ti.bios.BUF.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.BUF$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.BUF.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$BUF$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.BUF'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void LCK$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.LCK$$create", new Proto.Fxn(om.findStrict("ti.bios.LCK.Module", "ti.bios"), om.findStrict("ti.bios.LCK.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.LCK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$LCK$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.LCK'];\n");
            sb.append("var __inst = xdc.om['ti.bios.LCK.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.LCK$$construct", new Proto.Fxn(om.findStrict("ti.bios.LCK.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.LCK$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.LCK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$LCK$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.LCK'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void HOOK$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.HOOK$$create", new Proto.Fxn(om.findStrict("ti.bios.HOOK.Module", "ti.bios"), om.findStrict("ti.bios.HOOK.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.HOOK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HOOK$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HOOK'];\n");
            sb.append("var __inst = xdc.om['ti.bios.HOOK.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.HOOK$$construct", new Proto.Fxn(om.findStrict("ti.bios.HOOK.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.HOOK$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.HOOK.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HOOK$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HOOK'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.order = __mod.PARAMS.order;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void GBL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void HST$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.HST$$create", new Proto.Fxn(om.findStrict("ti.bios.HST.Module", "ti.bios"), om.findStrict("ti.bios.HST.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.HST.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HST$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HST'];\n");
            sb.append("var __inst = xdc.om['ti.bios.HST.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.mode = __mod.PARAMS.mode;\n");
            sb.append("__inst.bufSeg = __mod.PARAMS.bufSeg;\n");
            sb.append("__inst.bufAlign = __mod.PARAMS.bufAlign;\n");
            sb.append("__inst.frameSize = __mod.PARAMS.frameSize;\n");
            sb.append("__inst.numFrames = __mod.PARAMS.numFrames;\n");
            sb.append("__inst.statistics = __mod.PARAMS.statistics;\n");
            sb.append("__inst.availableForDHL = __mod.PARAMS.availableForDHL;\n");
            sb.append("__inst.notifyFxn = __mod.PARAMS.notifyFxn;\n");
            sb.append("__inst.arg0 = __mod.PARAMS.arg0;\n");
            sb.append("__inst.arg1 = __mod.PARAMS.arg1;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.HST$$construct", new Proto.Fxn(om.findStrict("ti.bios.HST.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.HST$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.HST.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$HST$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.HST'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.mode = __mod.PARAMS.mode;\n");
            sb.append("__inst.bufSeg = __mod.PARAMS.bufSeg;\n");
            sb.append("__inst.bufAlign = __mod.PARAMS.bufAlign;\n");
            sb.append("__inst.frameSize = __mod.PARAMS.frameSize;\n");
            sb.append("__inst.numFrames = __mod.PARAMS.numFrames;\n");
            sb.append("__inst.statistics = __mod.PARAMS.statistics;\n");
            sb.append("__inst.availableForDHL = __mod.PARAMS.availableForDHL;\n");
            sb.append("__inst.notifyFxn = __mod.PARAMS.notifyFxn;\n");
            sb.append("__inst.arg0 = __mod.PARAMS.arg0;\n");
            sb.append("__inst.arg1 = __mod.PARAMS.arg1;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void PIP$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.PIP$$create", new Proto.Fxn(om.findStrict("ti.bios.PIP.Module", "ti.bios"), om.findStrict("ti.bios.PIP.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.PIP.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PIP$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PIP'];\n");
            sb.append("var __inst = xdc.om['ti.bios.PIP.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.PIP$$construct", new Proto.Fxn(om.findStrict("ti.bios.PIP.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.PIP$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.PIP.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$PIP$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.PIP'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.monitor = __mod.PARAMS.monitor;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void RTDX$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.RTDX$$create", new Proto.Fxn(om.findStrict("ti.bios.RTDX.Module", "ti.bios"), om.findStrict("ti.bios.RTDX.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.RTDX.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$RTDX$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.RTDX'];\n");
            sb.append("var __inst = xdc.om['ti.bios.RTDX.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.channelMode = __mod.PARAMS.channelMode;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.RTDX$$construct", new Proto.Fxn(om.findStrict("ti.bios.RTDX.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.RTDX$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.RTDX.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$RTDX$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.RTDX'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.channelMode = __mod.PARAMS.channelMode;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void ECM$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.ECM$$create", new Proto.Fxn(om.findStrict("ti.bios.ECM.Module", "ti.bios"), om.findStrict("ti.bios.ECM.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.ECM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$ECM$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.ECM'];\n");
            sb.append("var __inst = xdc.om['ti.bios.ECM.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.ECM$$construct", new Proto.Fxn(om.findStrict("ti.bios.ECM.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.ECM$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.ECM.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$ECM$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.ECM'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DEV$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.DEV$$create", new Proto.Fxn(om.findStrict("ti.bios.DEV.Module", "ti.bios"), om.findStrict("ti.bios.DEV.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.DEV.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DEV$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DEV'];\n");
            sb.append("var __inst = xdc.om['ti.bios.DEV.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.DEV$$construct", new Proto.Fxn(om.findStrict("ti.bios.DEV.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.DEV$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.DEV.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DEV$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DEV'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DGN$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.DGN$$create", new Proto.Fxn(om.findStrict("ti.bios.DGN.Module", "ti.bios"), om.findStrict("ti.bios.DGN.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.DGN.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DGN$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DGN'];\n");
            sb.append("var __inst = xdc.om['ti.bios.DGN.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.DGN$$construct", new Proto.Fxn(om.findStrict("ti.bios.DGN.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.DGN$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.DGN.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DGN$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DGN'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DHL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.DHL$$create", new Proto.Fxn(om.findStrict("ti.bios.DHL.Module", "ti.bios"), om.findStrict("ti.bios.DHL.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.DHL.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DHL$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DHL'];\n");
            sb.append("var __inst = xdc.om['ti.bios.DHL.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("__inst.name = __mod.PARAMS.name;\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.hstChannel = __mod.PARAMS.hstChannel;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.DHL$$construct", new Proto.Fxn(om.findStrict("ti.bios.DHL.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.DHL$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.DHL.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DHL$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DHL'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("__inst.name = __mod.PARAMS.name;\n");
            sb.append("__inst.comment = __mod.PARAMS.comment;\n");
            sb.append("__inst.hstChannel = __mod.PARAMS.hstChannel;\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DPI$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.DPI$$create", new Proto.Fxn(om.findStrict("ti.bios.DPI.Module", "ti.bios"), om.findStrict("ti.bios.DPI.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.DPI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DPI$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DPI'];\n");
            sb.append("var __inst = xdc.om['ti.bios.DPI.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.DPI$$construct", new Proto.Fxn(om.findStrict("ti.bios.DPI.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.DPI$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.DPI.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DPI$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DPI'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void DIO$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.DIO$$create", new Proto.Fxn(om.findStrict("ti.bios.DIO.Module", "ti.bios"), om.findStrict("ti.bios.DIO.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.DIO.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DIO$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DIO'];\n");
            sb.append("var __inst = xdc.om['ti.bios.DIO.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.DIO$$construct", new Proto.Fxn(om.findStrict("ti.bios.DIO.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.DIO$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.DIO.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$DIO$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.DIO'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void GIO$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SIO$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.SIO$$create", new Proto.Fxn(om.findStrict("ti.bios.SIO.Module", "ti.bios"), om.findStrict("ti.bios.SIO.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.SIO.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SIO$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SIO'];\n");
            sb.append("var __inst = xdc.om['ti.bios.SIO.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.SIO$$construct", new Proto.Fxn(om.findStrict("ti.bios.SIO.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.SIO$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.SIO.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$SIO$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.SIO'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void UDEV$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

        fxn = (Proto.Fxn)om.bind("ti.bios.UDEV$$create", new Proto.Fxn(om.findStrict("ti.bios.UDEV.Module", "ti.bios"), om.findStrict("ti.bios.UDEV.Instance", "ti.bios"), 2, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
                fxn.addArg(1, "__params", (Proto)om.findStrict("ti.bios.UDEV.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$UDEV$$create = function( name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.UDEV'];\n");
            sb.append("var __inst = xdc.om['ti.bios.UDEV.Instance'].$$make();\n");
            sb.append("__inst.$$bind('$package', xdc.om['ti.bios']);\n");
            sb.append("__inst.$$bind('$index', __mod.$instances.length);\n");
            sb.append("__inst.$$bind('$category', 'Instance');\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$instances.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("var save = xdc.om.$curpkg;\n");
            sb.append("xdc.om.$$bind('$curpkg', __mod.$package.$name);\n");
            sb.append("__mod.instance$meta$init.$fxn.apply(__inst, [name]);\n");
            sb.append("xdc.om.$$bind('$curpkg', save);\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return __inst;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
        fxn = (Proto.Fxn)om.bind("ti.bios.UDEV$$construct", new Proto.Fxn(om.findStrict("ti.bios.UDEV.Module", "ti.bios"), null, 3, 1, false));
                fxn.addArg(0, "__obj", (Proto)om.findStrict("ti.bios.UDEV$$Object", "ti.bios"), null);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
                fxn.addArg(2, "__params", (Proto)om.findStrict("ti.bios.UDEV.Params", "ti.bios"), Global.newObject());
        sb = new StringBuilder();
        sb.append("ti$bios$UDEV$$construct = function( __obj, name, __params ) {\n");
            sb.append("var __mod = xdc.om['ti.bios.UDEV'];\n");
            sb.append("var __inst = __obj;\n");
            sb.append("__inst.$$bind('$args', {name:name});\n");
            sb.append("__inst.$$bind('$module', __mod);\n");
            sb.append("__mod.$objects.$add(__inst);\n");
            sb.append("for (var __p in __params) __inst[__p] = __params[__p];\n");
            sb.append("__inst.$$bless();\n");
            sb.append("return null;\n");
        sb.append("}\n");
        Global.eval(sb.toString());
    }

    void MSGQ$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void POOL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void STATICPOOL$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SYS$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SYS.close
        fxn = (Proto.Fxn)om.bind("ti.bios.SYS$$close", new Proto.Fxn(om.findStrict("ti.bios.SYS.Module", "ti.bios"), null, 0, -1, false));
        // fxn SYS.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.SYS$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.SYS.Module", "ti.bios"), null, 0, -1, false));
    }

    void LOG$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LOG.close
        fxn = (Proto.Fxn)om.bind("ti.bios.LOG$$close", new Proto.Fxn(om.findStrict("ti.bios.LOG.Module", "ti.bios"), null, 0, -1, false));
        // fxn LOG.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.LOG$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.LOG.Module", "ti.bios"), null, 0, -1, false));
        // fxn LOG.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.LOG$$destroy", new Proto.Fxn(om.findStrict("ti.bios.LOG.Instance", "ti.bios"), null, 0, 0, false));
    }

    void MEM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn MEM.addHeapMem
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$addHeapMem", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), 2, 2, false));
                fxn.addArg(0, "handle", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Handle", "ti.bios"), $$UNDEF);
                fxn.addArg(1, "name", $$T_Str, $$UNDEF);
        // fxn MEM.getHeapHandle
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$getHeapHandle", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), (Proto)om.findStrict("xdc.runtime.IHeap.Handle", "ti.bios"), 1, 1, false));
                fxn.addArg(0, "segment", $$T_Str, $$UNDEF);
        // fxn MEM.inst
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$inst", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), 1, 1, false));
                fxn.addArg(0, "name", $$T_Str, $$UNDEF);
        // fxn MEM.close
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$close", new Proto.Fxn(om.findStrict("ti.bios.MEM.Module", "ti.bios"), null, 0, -1, false));
        // fxn MEM.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$destroy", new Proto.Fxn(om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, 0, 0, false));
        // fxn MEM.loadRunHandling
        fxn = (Proto.Fxn)om.bind("ti.bios.MEM$$loadRunHandling", new Proto.Fxn(om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, 5, 5, false));
                fxn.addArg(0, "load", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF);
                fxn.addArg(1, "loadStr", $$T_Str, $$UNDEF);
                fxn.addArg(2, "run", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF);
                fxn.addArg(3, "runStr", $$T_Str, $$UNDEF);
                fxn.addArg(4, "sectionName", $$T_Str, $$UNDEF);
    }

    void TSK$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn TSK.close
        fxn = (Proto.Fxn)om.bind("ti.bios.TSK$$close", new Proto.Fxn(om.findStrict("ti.bios.TSK.Module", "ti.bios"), null, 0, -1, false));
        // fxn TSK.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.TSK$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.TSK.Module", "ti.bios"), null, 0, -1, false));
        // fxn TSK.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.TSK$$destroy", new Proto.Fxn(om.findStrict("ti.bios.TSK.Instance", "ti.bios"), null, 0, 0, false));
    }

    void CLK$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn CLK.close
        fxn = (Proto.Fxn)om.bind("ti.bios.CLK$$close", new Proto.Fxn(om.findStrict("ti.bios.CLK.Module", "ti.bios"), null, 0, -1, false));
        // fxn CLK.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.CLK$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.CLK.Module", "ti.bios"), null, 0, -1, false));
        // fxn CLK.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.CLK$$destroy", new Proto.Fxn(om.findStrict("ti.bios.CLK.Instance", "ti.bios"), null, 0, 0, false));
    }

    void SEM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SEM.close
        fxn = (Proto.Fxn)om.bind("ti.bios.SEM$$close", new Proto.Fxn(om.findStrict("ti.bios.SEM.Module", "ti.bios"), null, 0, -1, false));
        // fxn SEM.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.SEM$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.SEM.Module", "ti.bios"), null, 0, -1, false));
        // fxn SEM.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.SEM$$destroy", new Proto.Fxn(om.findStrict("ti.bios.SEM.Instance", "ti.bios"), null, 0, 0, false));
    }

    void SWI$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SWI.close
        fxn = (Proto.Fxn)om.bind("ti.bios.SWI$$close", new Proto.Fxn(om.findStrict("ti.bios.SWI.Module", "ti.bios"), null, 0, -1, false));
        // fxn SWI.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.SWI$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.SWI.Module", "ti.bios"), null, 0, -1, false));
        // fxn SWI.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.SWI$$destroy", new Proto.Fxn(om.findStrict("ti.bios.SWI.Instance", "ti.bios"), null, 0, 0, false));
    }

    void IDL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn IDL.close
        fxn = (Proto.Fxn)om.bind("ti.bios.IDL$$close", new Proto.Fxn(om.findStrict("ti.bios.IDL.Module", "ti.bios"), null, 0, -1, false));
        // fxn IDL.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.IDL$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.IDL.Module", "ti.bios"), null, 0, -1, false));
        // fxn IDL.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.IDL$$destroy", new Proto.Fxn(om.findStrict("ti.bios.IDL.Instance", "ti.bios"), null, 0, 0, false));
    }

    void MBX$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn MBX.close
        fxn = (Proto.Fxn)om.bind("ti.bios.MBX$$close", new Proto.Fxn(om.findStrict("ti.bios.MBX.Module", "ti.bios"), null, 0, -1, false));
        // fxn MBX.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.MBX$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.MBX.Module", "ti.bios"), null, 0, -1, false));
        // fxn MBX.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.MBX$$destroy", new Proto.Fxn(om.findStrict("ti.bios.MBX.Instance", "ti.bios"), null, 0, 0, false));
    }

    void STS$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn STS.close
        fxn = (Proto.Fxn)om.bind("ti.bios.STS$$close", new Proto.Fxn(om.findStrict("ti.bios.STS.Module", "ti.bios"), null, 0, -1, false));
        // fxn STS.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.STS$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.STS.Module", "ti.bios"), null, 0, -1, false));
        // fxn STS.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.STS$$destroy", new Proto.Fxn(om.findStrict("ti.bios.STS.Instance", "ti.bios"), null, 0, 0, false));
    }

    void QUE$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn QUE.close
        fxn = (Proto.Fxn)om.bind("ti.bios.QUE$$close", new Proto.Fxn(om.findStrict("ti.bios.QUE.Module", "ti.bios"), null, 0, -1, false));
        // fxn QUE.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.QUE$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.QUE.Module", "ti.bios"), null, 0, -1, false));
        // fxn QUE.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.QUE$$destroy", new Proto.Fxn(om.findStrict("ti.bios.QUE.Instance", "ti.bios"), null, 0, 0, false));
    }

    void PRD$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn PRD.close
        fxn = (Proto.Fxn)om.bind("ti.bios.PRD$$close", new Proto.Fxn(om.findStrict("ti.bios.PRD.Module", "ti.bios"), null, 0, -1, false));
        // fxn PRD.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.PRD$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.PRD.Module", "ti.bios"), null, 0, -1, false));
        // fxn PRD.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.PRD$$destroy", new Proto.Fxn(om.findStrict("ti.bios.PRD.Instance", "ti.bios"), null, 0, 0, false));
    }

    void HWI$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn HWI.close
        fxn = (Proto.Fxn)om.bind("ti.bios.HWI$$close", new Proto.Fxn(om.findStrict("ti.bios.HWI.Module", "ti.bios"), null, 0, -1, false));
        // fxn HWI.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.HWI$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.HWI.Module", "ti.bios"), null, 0, -1, false));
    }

    void PIE$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn PIE.close
        fxn = (Proto.Fxn)om.bind("ti.bios.PIE$$close", new Proto.Fxn(om.findStrict("ti.bios.PIE.Module", "ti.bios"), null, 0, -1, false));
        // fxn PIE.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.PIE$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.PIE.Module", "ti.bios"), null, 0, -1, false));
    }

    void BUF$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn BUF.close
        fxn = (Proto.Fxn)om.bind("ti.bios.BUF$$close", new Proto.Fxn(om.findStrict("ti.bios.BUF.Module", "ti.bios"), null, 0, -1, false));
        // fxn BUF.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.BUF$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.BUF.Module", "ti.bios"), null, 0, -1, false));
        // fxn BUF.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.BUF$$destroy", new Proto.Fxn(om.findStrict("ti.bios.BUF.Instance", "ti.bios"), null, 0, 0, false));
    }

    void LCK$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn LCK.close
        fxn = (Proto.Fxn)om.bind("ti.bios.LCK$$close", new Proto.Fxn(om.findStrict("ti.bios.LCK.Module", "ti.bios"), null, 0, -1, false));
        // fxn LCK.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.LCK$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.LCK.Module", "ti.bios"), null, 0, -1, false));
        // fxn LCK.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.LCK$$destroy", new Proto.Fxn(om.findStrict("ti.bios.LCK.Instance", "ti.bios"), null, 0, 0, false));
    }

    void HOOK$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn HOOK.close
        fxn = (Proto.Fxn)om.bind("ti.bios.HOOK$$close", new Proto.Fxn(om.findStrict("ti.bios.HOOK.Module", "ti.bios"), null, 0, -1, false));
        // fxn HOOK.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.HOOK$$destroy", new Proto.Fxn(om.findStrict("ti.bios.HOOK.Instance", "ti.bios"), null, 0, 0, false));
    }

    void GBL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn GBL.close
        fxn = (Proto.Fxn)om.bind("ti.bios.GBL$$close", new Proto.Fxn(om.findStrict("ti.bios.GBL.Module", "ti.bios"), null, 0, -1, false));
        // fxn GBL.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.GBL$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.GBL.Module", "ti.bios"), null, 0, -1, false));
    }

    void HST$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn HST.close
        fxn = (Proto.Fxn)om.bind("ti.bios.HST$$close", new Proto.Fxn(om.findStrict("ti.bios.HST.Module", "ti.bios"), null, 0, -1, false));
        // fxn HST.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.HST$$destroy", new Proto.Fxn(om.findStrict("ti.bios.HST.Instance", "ti.bios"), null, 0, 0, false));
    }

    void PIP$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn PIP.close
        fxn = (Proto.Fxn)om.bind("ti.bios.PIP$$close", new Proto.Fxn(om.findStrict("ti.bios.PIP.Module", "ti.bios"), null, 0, -1, false));
        // fxn PIP.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.PIP$$destroy", new Proto.Fxn(om.findStrict("ti.bios.PIP.Instance", "ti.bios"), null, 0, 0, false));
    }

    void RTDX$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn RTDX.close
        fxn = (Proto.Fxn)om.bind("ti.bios.RTDX$$close", new Proto.Fxn(om.findStrict("ti.bios.RTDX.Module", "ti.bios"), null, 0, -1, false));
        // fxn RTDX.destroy
        fxn = (Proto.Fxn)om.bind("ti.bios.RTDX$$destroy", new Proto.Fxn(om.findStrict("ti.bios.RTDX.Instance", "ti.bios"), null, 0, 0, false));
    }

    void ECM$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ECM.close
        fxn = (Proto.Fxn)om.bind("ti.bios.ECM$$close", new Proto.Fxn(om.findStrict("ti.bios.ECM.Module", "ti.bios"), null, 0, -1, false));
        // fxn ECM.exportMod
        fxn = (Proto.Fxn)om.bind("ti.bios.ECM$$exportMod", new Proto.Fxn(om.findStrict("ti.bios.ECM.Module", "ti.bios"), null, 0, -1, false));
    }

    void DEV$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn DEV.close
        fxn = (Proto.Fxn)om.bind("ti.bios.DEV$$close", new Proto.Fxn(om.findStrict("ti.bios.DEV.Module", "ti.bios"), null, 0, -1, false));
    }

    void DGN$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn DGN.close
        fxn = (Proto.Fxn)om.bind("ti.bios.DGN$$close", new Proto.Fxn(om.findStrict("ti.bios.DGN.Module", "ti.bios"), null, 0, -1, false));
    }

    void DHL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn DHL.close
        fxn = (Proto.Fxn)om.bind("ti.bios.DHL$$close", new Proto.Fxn(om.findStrict("ti.bios.DHL.Module", "ti.bios"), null, 0, -1, false));
    }

    void DPI$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn DPI.close
        fxn = (Proto.Fxn)om.bind("ti.bios.DPI$$close", new Proto.Fxn(om.findStrict("ti.bios.DPI.Module", "ti.bios"), null, 0, -1, false));
    }

    void DIO$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn DIO.close
        fxn = (Proto.Fxn)om.bind("ti.bios.DIO$$close", new Proto.Fxn(om.findStrict("ti.bios.DIO.Module", "ti.bios"), null, 0, -1, false));
    }

    void GIO$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn GIO.close
        fxn = (Proto.Fxn)om.bind("ti.bios.GIO$$close", new Proto.Fxn(om.findStrict("ti.bios.GIO.Module", "ti.bios"), null, 0, -1, false));
    }

    void SIO$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn SIO.close
        fxn = (Proto.Fxn)om.bind("ti.bios.SIO$$close", new Proto.Fxn(om.findStrict("ti.bios.SIO.Module", "ti.bios"), null, 0, -1, false));
    }

    void UDEV$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn UDEV.close
        fxn = (Proto.Fxn)om.bind("ti.bios.UDEV$$close", new Proto.Fxn(om.findStrict("ti.bios.UDEV.Module", "ti.bios"), null, 0, -1, false));
        // fxn UDEV.get
        fxn = (Proto.Fxn)om.bind("ti.bios.UDEV$$get", new Proto.Fxn(om.findStrict("ti.bios.UDEV.Module", "ti.bios"), null, 0, -1, false));
    }

    void MSGQ$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn MSGQ.close
        fxn = (Proto.Fxn)om.bind("ti.bios.MSGQ$$close", new Proto.Fxn(om.findStrict("ti.bios.MSGQ.Module", "ti.bios"), null, 0, -1, false));
    }

    void POOL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn POOL.close
        fxn = (Proto.Fxn)om.bind("ti.bios.POOL$$close", new Proto.Fxn(om.findStrict("ti.bios.POOL.Module", "ti.bios"), null, 0, -1, false));
    }

    void STATICPOOL$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn STATICPOOL.close
        fxn = (Proto.Fxn)om.bind("ti.bios.STATICPOOL$$close", new Proto.Fxn(om.findStrict("ti.bios.STATICPOOL.Module", "ti.bios"), null, 0, -1, false));
    }

    void SYS$$SIZES()
    {
    }

    void LOG$$SIZES()
    {
    }

    void MEM$$SIZES()
    {
    }

    void TSK$$SIZES()
    {
    }

    void CLK$$SIZES()
    {
    }

    void SEM$$SIZES()
    {
    }

    void SWI$$SIZES()
    {
    }

    void IDL$$SIZES()
    {
    }

    void MBX$$SIZES()
    {
    }

    void STS$$SIZES()
    {
    }

    void QUE$$SIZES()
    {
    }

    void PRD$$SIZES()
    {
    }

    void HWI$$SIZES()
    {
    }

    void PIE$$SIZES()
    {
    }

    void BUF$$SIZES()
    {
    }

    void LCK$$SIZES()
    {
    }

    void HOOK$$SIZES()
    {
    }

    void GBL$$SIZES()
    {
    }

    void HST$$SIZES()
    {
    }

    void PIP$$SIZES()
    {
    }

    void RTDX$$SIZES()
    {
    }

    void ECM$$SIZES()
    {
    }

    void DEV$$SIZES()
    {
    }

    void DGN$$SIZES()
    {
    }

    void DHL$$SIZES()
    {
    }

    void DPI$$SIZES()
    {
    }

    void DIO$$SIZES()
    {
    }

    void GIO$$SIZES()
    {
    }

    void SIO$$SIZES()
    {
    }

    void UDEV$$SIZES()
    {
    }

    void MSGQ$$SIZES()
    {
    }

    void POOL$$SIZES()
    {
    }

    void STATICPOOL$$SIZES()
    {
    }

    void SYS$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/SYS.xs");
        om.bind("ti.bios.SYS$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.SYS.Module", "ti.bios");
        po.init("ti.bios.SYS.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("TRACESIZE", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
        po.addFld("TRACESEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("ABORTFXN", $$T_Str, null, "wh");
        po.addFld("ERRORFXN", $$T_Str, null, "wh");
        po.addFld("EXITFXN", $$T_Str, null, "wh");
        po.addFld("PUTCFXN", $$T_Str, null, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.SYS$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.SYS$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.SYS$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.SYS$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.SYS$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
    }

    void LOG$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/LOG.xs");
        om.bind("ti.bios.LOG$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.LOG.Module", "ti.bios");
        po.init("ti.bios.LOG.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("enableFlush", $$T_Bool, false, "wh");
        po.addFld("TS", $$T_Bool, $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.LOG$$create", "ti.bios"), Global.get("ti$bios$LOG$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.LOG$$construct", "ti.bios"), Global.get("ti$bios$LOG$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.LOG$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.LOG$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.LOG$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.LOG$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.LOG$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.LOG$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.LOG.Instance", "ti.bios");
        po.init("ti.bios.LOG.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufLen", Proto.Elm.newCNum("(xdc_UInt)"), 64L, "wh");
        po.addFld("logType", $$T_Str, "circular", "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("format", $$T_Str, $$UNDEF, "wh");
        po.addFld("exitFlush", $$T_Bool, false, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.LOG$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.LOG$$Params", "ti.bios");
        po.init("ti.bios.LOG.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufLen", Proto.Elm.newCNum("(xdc_UInt)"), 64L, "wh");
        po.addFld("logType", $$T_Str, "circular", "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("format", $$T_Str, $$UNDEF, "wh");
        po.addFld("exitFlush", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.LOG$$Object", "ti.bios");
        po.init("ti.bios.LOG.Object", om.findStrict("ti.bios.LOG.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.LOG$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void MEM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/MEM.xs");
        om.bind("ti.bios.MEM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.MEM.Module", "ti.bios");
        po.init("ti.bios.MEM.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("NOMEMORYHEAPS", $$T_Bool, false, "wh");
        po.addFld("MALLOCSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("BIOSOBJSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("ARGSSIZE", Proto.Elm.newCNum("(xdc_UInt)"), 8L, "wh");
        po.addFld("ARGSSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("STACKSIZE", Proto.Elm.newCNum("(xdc_UInt)"), 0x400L, "wh");
        po.addFld("STACKSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("GBLINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("TRCDATASEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("SYSDATASEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("OBJSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("BIOSSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("SYSINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("HWISEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("HWIVECSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("RTDXTEXTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("USERCOMMANDFILE", $$T_Bool, false, "wh");
        po.addFld("TEXTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("DATASEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("SWITCHSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("BSSSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("EBSSSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("FARSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("CINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("PINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("CONSTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("ECONSTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("CIOSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("ENABLELOADADDR", $$T_Bool, false, "wh");
        po.addFld("LOADBIOSSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADSYSINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADGBLINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADTRCDATASEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADTEXTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADSWITCHSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADCINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADPINITSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADCONSTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADECONSTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADHWISEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADHWIVECSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("LOADRTDXTEXTSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("currentNumHeapMems", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("heapList", new Proto.Arr((Proto)om.findStrict("ti.bios.MEM.HeapList", "ti.bios"), false), $$DEFAULT, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.MEM$$create", "ti.bios"), Global.get("ti$bios$MEM$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.MEM$$construct", "ti.bios"), Global.get("ti$bios$MEM$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.MEM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.MEM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.MEM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.MEM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("addHeapMem", (Proto.Fxn)om.findStrict("ti.bios.MEM$$addHeapMem", "ti.bios"), Global.get(cap, "addHeapMem"));
                po.addFxn("getHeapHandle", (Proto.Fxn)om.findStrict("ti.bios.MEM$$getHeapHandle", "ti.bios"), Global.get(cap, "getHeapHandle"));
                po.addFxn("inst", (Proto.Fxn)om.findStrict("ti.bios.MEM$$inst", "ti.bios"), Global.get(cap, "inst"));
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.MEM$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.MEM.Instance", "ti.bios");
        po.init("ti.bios.MEM.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("base", Proto.Elm.newCNum("(xdc_UInt)"), 0x00000000L, "wh");
        po.addFld("len", Proto.Elm.newCNum("(xdc_UInt)"), 0x00000000L, "wh");
        po.addFld("createHeap", $$T_Bool, false, "wh");
        po.addFld("heapSize", Proto.Elm.newCNum("(xdc_UInt)"), 0x08000L, "wh");
        po.addFld("enableHeapLabel", $$T_Bool, false, "wh");
        po.addFld("heapLabel", $$T_Str, null, "wh");
        po.addFld("space", $$T_Str, "data", "wh");
        po.addFld("heapId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("heapMemInst", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Handle", "ti.bios"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.MEM$$destroy", "ti.bios"), Global.get(cap, "destroy"));
                po.addFxn("loadRunHandling", (Proto.Fxn)om.findStrict("ti.bios.MEM$$loadRunHandling", "ti.bios"), Global.get(cap, "loadRunHandling"));
        po = (Proto.Obj)om.findStrict("ti.bios.MEM$$Params", "ti.bios");
        po.init("ti.bios.MEM.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("base", Proto.Elm.newCNum("(xdc_UInt)"), 0x00000000L, "wh");
        po.addFld("len", Proto.Elm.newCNum("(xdc_UInt)"), 0x00000000L, "wh");
        po.addFld("createHeap", $$T_Bool, false, "wh");
        po.addFld("heapSize", Proto.Elm.newCNum("(xdc_UInt)"), 0x08000L, "wh");
        po.addFld("enableHeapLabel", $$T_Bool, false, "wh");
        po.addFld("heapLabel", $$T_Str, null, "wh");
        po.addFld("space", $$T_Str, "data", "wh");
        po.addFld("heapId", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("heapMemInst", (Proto)om.findStrict("ti.sysbios.heaps.HeapMem.Handle", "ti.bios"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.MEM$$Object", "ti.bios");
        po.init("ti.bios.MEM.Object", om.findStrict("ti.bios.MEM.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.MEM$$destroy", "ti.bios"), Global.get(cap, "destroy"));
                po.addFxn("loadRunHandling", (Proto.Fxn)om.findStrict("ti.bios.MEM$$loadRunHandling", "ti.bios"), Global.get(cap, "loadRunHandling"));
        // struct MEM.HeapList
        po = (Proto.Obj)om.findStrict("ti.bios.MEM$$HeapList", "ti.bios");
        po.init("ti.bios.MEM.HeapList", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("heapLabel", $$T_Str, $$UNDEF, "w");
                po.addFld("heapMemIndex", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "w");
    }

    void TSK$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/TSK.xs");
        om.bind("ti.bios.TSK$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.TSK.Module", "ti.bios");
        po.init("ti.bios.TSK.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLETSK", $$T_Bool, true, "wh");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("STACKSIZE", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("STACKSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("PRIORITY", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("DRIVETSKTICK", $$T_Str, $$UNDEF, "wh");
        po.addFld("CREATEFXN", $$T_Str, "FXN_F_nop", "wh");
        po.addFld("DELETEFXN", $$T_Str, "FXN_F_nop", "wh");
        po.addFld("EXITFXN", $$T_Str, "FXN_F_nop", "wh");
        po.addFld("CALLSWITCHFXN", $$T_Bool, $$UNDEF, "wh");
        po.addFld("SWITCHFXN", $$T_Str, $$UNDEF, "wh");
        po.addFld("CALLREADYFXN", $$T_Bool, false, "wh");
        po.addFld("READYFXN", $$T_Str, "FXN_F_nop", "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.TSK$$create", "ti.bios"), Global.get("ti$bios$TSK$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.TSK$$construct", "ti.bios"), Global.get("ti$bios$TSK$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.TSK$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.TSK$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.TSK$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.TSK$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.TSK$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.TSK$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.TSK.Instance", "ti.bios");
        po.init("ti.bios.TSK.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("autoAllocateStack", $$T_Bool, true, "wh");
        po.addFld("manualStack", $$T_Str, $$UNDEF, "wh");
        po.addFld("stackSize", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("stackMemSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg2", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg3", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg4", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg5", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg6", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg7", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("envPointer", new Proto.Adr("xdc_UArg", "Pv"), null, "wh");
        po.addFld("exitFlag", $$T_Bool, true, "wh");
        po.addFld("allocateTaskName", $$T_Bool, false, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.TSK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.TSK$$Params", "ti.bios");
        po.init("ti.bios.TSK.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("autoAllocateStack", $$T_Bool, true, "wh");
        po.addFld("manualStack", $$T_Str, $$UNDEF, "wh");
        po.addFld("stackSize", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("stackMemSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg2", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg3", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg4", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg5", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg6", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg7", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("envPointer", new Proto.Adr("xdc_UArg", "Pv"), null, "wh");
        po.addFld("exitFlag", $$T_Bool, true, "wh");
        po.addFld("allocateTaskName", $$T_Bool, false, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.TSK$$Object", "ti.bios");
        po.init("ti.bios.TSK.Object", om.findStrict("ti.bios.TSK.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.TSK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void CLK$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/CLK.xs");
        om.bind("ti.bios.CLK$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.CLK.Module", "ti.bios");
        po.init("ti.bios.CLK.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("ENABLECLK", $$T_Bool, true, "wh");
        po.addFld("TIMERSELECT", $$T_Str, $$UNDEF, "wh");
        po.addFld("HIRESTIME", $$T_Bool, $$UNDEF, "wh");
        po.addFld("ENABLEHTIME", $$T_Bool, true, "wh");
        po.addFld("MICROSECONDS", Proto.Elm.newCNum("(xdc_Float)"), 1000L, "wh");
        po.addFld("CONFIGURETIMER", $$T_Bool, $$UNDEF, "wh");
        po.addFld("PRD", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "wh");
        po.addFld("WHICHHIRESTIMER", $$T_Str, null, "wh");
        po.addFld("SPECIFYRATE", $$T_Bool, $$UNDEF, "wh");
        po.addFld("INPUTCLK", Proto.Elm.newCNum("(xdc_Float)"), $$UNDEF, "wh");
        po.addFld("RESETTIMER", $$T_Bool, $$UNDEF, "wh");
        po.addFld("TIMMODE", $$T_Str, "32-bit unchained", "wh");
        po.addFld("FIXTDDR", $$T_Bool, $$UNDEF, "wh");
        po.addFld("TCRTDDR", Proto.Elm.newCNum("(xdc_Int)"), $$UNDEF, "wh");
        po.addFld("POSTINITFXN", $$T_Str, $$UNDEF, "wh");
        po.addFld("CONONDEBUG", $$T_Bool, $$UNDEF, "wh");
        po.addFld("STARTBOTH", $$T_Bool, $$UNDEF, "wh");
        po.addFld("TIMER5BASE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "wh");
        po.addFld("TIMER6BASE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "wh");
        po.addFld("TIMER7BASE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "wh");
        po.addFld("TIMER8BASE", Proto.Elm.newCNum("(xdc_UInt32)"), $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.CLK$$create", "ti.bios"), Global.get("ti$bios$CLK$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.CLK$$construct", "ti.bios"), Global.get("ti$bios$CLK$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.CLK$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.CLK$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.CLK$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.CLK$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.CLK$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.CLK$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.CLK.Instance", "ti.bios");
        po.init("ti.bios.CLK.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.CLK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.CLK$$Params", "ti.bios");
        po.init("ti.bios.CLK.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_Int16)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.CLK$$Object", "ti.bios");
        po.init("ti.bios.CLK.Object", om.findStrict("ti.bios.CLK.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.CLK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void SEM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/SEM.xs");
        om.bind("ti.bios.SEM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.SEM.Module", "ti.bios");
        po.init("ti.bios.SEM.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.SEM$$create", "ti.bios"), Global.get("ti$bios$SEM$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.SEM$$construct", "ti.bios"), Global.get("ti$bios$SEM$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.SEM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.SEM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.SEM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.SEM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.SEM$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.SEM$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.SEM.Instance", "ti.bios");
        po.init("ti.bios.SEM.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("count", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.SEM$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.SEM$$Params", "ti.bios");
        po.init("ti.bios.SEM.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("count", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.SEM$$Object", "ti.bios");
        po.init("ti.bios.SEM.Object", om.findStrict("ti.bios.SEM.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.SEM$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void SWI$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/SWI.xs");
        om.bind("ti.bios.SWI$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.SWI.Module", "ti.bios");
        po.init("ti.bios.SWI.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.SWI$$create", "ti.bios"), Global.get("ti$bios$SWI$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.SWI$$construct", "ti.bios"), Global.get("ti$bios$SWI$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.SWI$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.SWI$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.SWI$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.SWI$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.SWI$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.SWI$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.SWI.Instance", "ti.bios");
        po.init("ti.bios.SWI.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("mailbox", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.SWI$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.SWI$$Params", "ti.bios");
        po.init("ti.bios.SWI.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("mailbox", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.SWI$$Object", "ti.bios");
        po.init("ti.bios.SWI.Object", om.findStrict("ti.bios.SWI.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.SWI$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void IDL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/IDL.xs");
        om.bind("ti.bios.IDL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.IDL.Module", "ti.bios");
        po.init("ti.bios.IDL.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("AUTOCALCULATE", $$T_Bool, $$UNDEF, "wh");
        po.addFld("LOOPINSTCOUNT", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.IDL$$create", "ti.bios"), Global.get("ti$bios$IDL$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.IDL$$construct", "ti.bios"), Global.get("ti$bios$IDL$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.IDL$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.IDL$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.IDL$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.IDL$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.IDL$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.IDL$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.IDL.Instance", "ti.bios");
        po.init("ti.bios.IDL.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("calibration", $$T_Bool, $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.IDL$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.IDL$$Params", "ti.bios");
        po.init("ti.bios.IDL.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("calibration", $$T_Bool, $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.IDL$$Object", "ti.bios");
        po.init("ti.bios.IDL.Object", om.findStrict("ti.bios.IDL.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.IDL$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void MBX$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/MBX.xs");
        om.bind("ti.bios.MBX$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.MBX.Module", "ti.bios");
        po.init("ti.bios.MBX.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.MBX$$create", "ti.bios"), Global.get("ti$bios$MBX$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.MBX$$construct", "ti.bios"), Global.get("ti$bios$MBX$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.MBX$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.MBX$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.MBX$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.MBX$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.MBX$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.MBX$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.MBX.Instance", "ti.bios");
        po.init("ti.bios.MBX.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("messageSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("messageLength", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("elementSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.MBX$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.MBX$$Params", "ti.bios");
        po.init("ti.bios.MBX.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("messageSize", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("messageLength", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("elementSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.MBX$$Object", "ti.bios");
        po.init("ti.bios.MBX.Object", om.findStrict("ti.bios.MBX.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.MBX$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void STS$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/STS.xs");
        om.bind("ti.bios.STS$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.STS.Module", "ti.bios");
        po.init("ti.bios.STS.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.STS$$create", "ti.bios"), Global.get("ti$bios$STS$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.STS$$construct", "ti.bios"), Global.get("ti$bios$STS$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.STS$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.STS$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.STS$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.STS$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.STS$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.STS$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.STS.Instance", "ti.bios");
        po.init("ti.bios.STS.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("unitType", $$T_Str, null, "wh");
        po.addFld("operation", $$T_Str, null, "wh");
        po.addFld("previousVal", Proto.Elm.newCNum("(xdc_Int32)"), 0L, "wh");
        po.addFld("numA", Proto.Elm.newCNum("(xdc_Int32)"), 1L, "wh");
        po.addFld("numB", Proto.Elm.newCNum("(xdc_Int32)"), 0L, "wh");
        po.addFld("numC", Proto.Elm.newCNum("(xdc_Int32)"), 1L, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.STS$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.STS$$Params", "ti.bios");
        po.init("ti.bios.STS.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("unitType", $$T_Str, null, "wh");
        po.addFld("operation", $$T_Str, null, "wh");
        po.addFld("previousVal", Proto.Elm.newCNum("(xdc_Int32)"), 0L, "wh");
        po.addFld("numA", Proto.Elm.newCNum("(xdc_Int32)"), 1L, "wh");
        po.addFld("numB", Proto.Elm.newCNum("(xdc_Int32)"), 0L, "wh");
        po.addFld("numC", Proto.Elm.newCNum("(xdc_Int32)"), 1L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.STS$$Object", "ti.bios");
        po.init("ti.bios.STS.Object", om.findStrict("ti.bios.STS.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.STS$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void QUE$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/QUE.xs");
        om.bind("ti.bios.QUE$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.QUE.Module", "ti.bios");
        po.init("ti.bios.QUE.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.QUE$$create", "ti.bios"), Global.get("ti$bios$QUE$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.QUE$$construct", "ti.bios"), Global.get("ti$bios$QUE$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.QUE$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.QUE$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.QUE$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.QUE$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.QUE$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.QUE$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.QUE.Instance", "ti.bios");
        po.init("ti.bios.QUE.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.QUE$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.QUE$$Params", "ti.bios");
        po.init("ti.bios.QUE.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.QUE$$Object", "ti.bios");
        po.init("ti.bios.QUE.Object", om.findStrict("ti.bios.QUE.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.QUE$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void PRD$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/PRD.xs");
        om.bind("ti.bios.PRD$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.PRD.Module", "ti.bios");
        po.init("ti.bios.PRD.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("USECLK", $$T_Bool, $$UNDEF, "wh");
        po.addFld("MICROSECONDS", Proto.Elm.newCNum("(xdc_Float)"), 0.0, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.PRD$$create", "ti.bios"), Global.get("ti$bios$PRD$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.PRD$$construct", "ti.bios"), Global.get("ti$bios$PRD$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.PRD$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.PRD$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.PRD$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.PRD$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.PRD$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.PRD$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.PRD.Instance", "ti.bios");
        po.init("ti.bios.PRD.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("mode", $$T_Str, "continuous", "wh");
        po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.PRD$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.PRD$$Params", "ti.bios");
        po.init("ti.bios.PRD.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("mode", $$T_Str, "continuous", "wh");
        po.addFld("period", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.PRD$$Object", "ti.bios");
        po.init("ti.bios.PRD.Object", om.findStrict("ti.bios.PRD.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.PRD$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void HWI$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/HWI.xs");
        om.bind("ti.bios.HWI$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.HWI.Module", "ti.bios");
        po.init("ti.bios.HWI.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("RESETVECTOR", $$T_Bool, $$UNDEF, "wh");
        po.addFld("RESETVECTORADDR", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("EXTPIN4POLARITY", $$T_Str, $$UNDEF, "wh");
        po.addFld("EXTPIN5POLARITY", $$T_Str, $$UNDEF, "wh");
        po.addFld("EXTPIN6POLARITY", $$T_Str, $$UNDEF, "wh");
        po.addFld("EXTPIN7POLARITY", $$T_Str, $$UNDEF, "wh");
        po.addFld("ENABLEEXC", $$T_Bool, $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.HWI$$create", "ti.bios"), Global.get("ti$bios$HWI$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.HWI$$construct", "ti.bios"), Global.get("ti$bios$HWI$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.HWI$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.HWI$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.HWI$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.HWI$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.HWI$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.HWI$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.HWI.Instance", "ti.bios");
        po.init("ti.bios.HWI.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("interruptSource", $$T_Str, null, "wh");
        po.addFld("interruptSelectNumber", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("fxn", $$T_Str, "HWI_unused", "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("addr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("operation", $$T_Str, $$UNDEF, "wh");
        po.addFld("useDispatcher", $$T_Bool, false, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0x00000000L, "wh");
        po.addFld("interruptMask", $$T_Str, "self", "wh");
        po.addFld("interruptBitMask", Proto.Elm.newCNum("(xdc_UInt16)"), 0x0000L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.HWI$$Params", "ti.bios");
        po.init("ti.bios.HWI.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("interruptSource", $$T_Str, null, "wh");
        po.addFld("interruptSelectNumber", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("fxn", $$T_Str, "HWI_unused", "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("addr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("operation", $$T_Str, $$UNDEF, "wh");
        po.addFld("useDispatcher", $$T_Bool, false, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0x00000000L, "wh");
        po.addFld("interruptMask", $$T_Str, "self", "wh");
        po.addFld("interruptBitMask", Proto.Elm.newCNum("(xdc_UInt16)"), 0x0000L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.HWI$$Object", "ti.bios");
        po.init("ti.bios.HWI.Object", om.findStrict("ti.bios.HWI.Instance", "ti.bios"));
    }

    void PIE$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/PIE.xs");
        om.bind("ti.bios.PIE$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.PIE.Module", "ti.bios");
        po.init("ti.bios.PIE.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("VMAPMODE", $$T_Bool, null, "wh");
        po.addFld("PIESELECT", $$T_Bool, null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.PIE$$create", "ti.bios"), Global.get("ti$bios$PIE$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.PIE$$construct", "ti.bios"), Global.get("ti$bios$PIE$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.PIE$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.PIE$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.PIE$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.PIE$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.PIE$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.PIE$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.PIE.Instance", "ti.bios");
        po.init("ti.bios.PIE.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, "HWI_unused", "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("addr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("operation", $$T_Str, $$UNDEF, "wh");
        po.addFld("useDispatcher", $$T_Bool, false, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0x00000000L, "wh");
        po.addFld("interruptMask0", $$T_Str, "self", "wh");
        po.addFld("interruptBitMask", Proto.Elm.newCNum("(xdc_UInt16)"), 0x0000L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.PIE$$Params", "ti.bios");
        po.init("ti.bios.PIE.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, "HWI_unused", "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("addr", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("dataType", $$T_Str, $$UNDEF, "wh");
        po.addFld("operation", $$T_Str, $$UNDEF, "wh");
        po.addFld("useDispatcher", $$T_Bool, false, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0x00000000L, "wh");
        po.addFld("interruptMask0", $$T_Str, "self", "wh");
        po.addFld("interruptBitMask", Proto.Elm.newCNum("(xdc_UInt16)"), 0x0000L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.PIE$$Object", "ti.bios");
        po.init("ti.bios.PIE.Object", om.findStrict("ti.bios.PIE.Instance", "ti.bios"));
    }

    void BUF$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/BUF.xs");
        om.bind("ti.bios.BUF$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.BUF.Module", "ti.bios");
        po.init("ti.bios.BUF.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.BUF$$create", "ti.bios"), Global.get("ti$bios$BUF$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.BUF$$construct", "ti.bios"), Global.get("ti$bios$BUF$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.BUF$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.BUF$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.BUF$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.BUF$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.BUF$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.BUF$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.BUF.Instance", "ti.bios");
        po.init("ti.bios.BUF.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufCount", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("size", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("align", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.BUF$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.BUF$$Params", "ti.bios");
        po.init("ti.bios.BUF.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufCount", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("size", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("align", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.BUF$$Object", "ti.bios");
        po.init("ti.bios.BUF.Object", om.findStrict("ti.bios.BUF.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.BUF$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void LCK$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/LCK.xs");
        om.bind("ti.bios.LCK$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.LCK.Module", "ti.bios");
        po.init("ti.bios.LCK.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.LCK$$create", "ti.bios"), Global.get("ti$bios$LCK$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.LCK$$construct", "ti.bios"), Global.get("ti$bios$LCK$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.LCK$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.LCK$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.LCK$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.LCK$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.LCK$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.LCK$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.LCK.Instance", "ti.bios");
        po.init("ti.bios.LCK.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.LCK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.LCK$$Params", "ti.bios");
        po.init("ti.bios.LCK.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.LCK$$Object", "ti.bios");
        po.init("ti.bios.LCK.Object", om.findStrict("ti.bios.LCK.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.LCK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void HOOK$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/HOOK.xs");
        om.bind("ti.bios.HOOK$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.HOOK.Module", "ti.bios");
        po.init("ti.bios.HOOK.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.HOOK$$create", "ti.bios"), Global.get("ti$bios$HOOK$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.HOOK$$construct", "ti.bios"), Global.get("ti$bios$HOOK$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.HOOK$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.HOOK$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.HOOK$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.HOOK$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.HOOK$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.HOOK.Instance", "ti.bios");
        po.init("ti.bios.HOOK.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("createFxn", $$T_Str, null, "wh");
        po.addFld("deleteFxn", $$T_Str, null, "wh");
        po.addFld("exitFxn", $$T_Str, null, "wh");
        po.addFld("callSwitchFxn", $$T_Bool, false, "wh");
        po.addFld("switchFxn", $$T_Str, null, "wh");
        po.addFld("callReadyFxn", $$T_Bool, false, "wh");
        po.addFld("readyFxn", $$T_Str, null, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.HOOK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.HOOK$$Params", "ti.bios");
        po.init("ti.bios.HOOK.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("createFxn", $$T_Str, null, "wh");
        po.addFld("deleteFxn", $$T_Str, null, "wh");
        po.addFld("exitFxn", $$T_Str, null, "wh");
        po.addFld("callSwitchFxn", $$T_Bool, false, "wh");
        po.addFld("switchFxn", $$T_Str, null, "wh");
        po.addFld("callReadyFxn", $$T_Bool, false, "wh");
        po.addFld("readyFxn", $$T_Str, null, "wh");
        po.addFld("order", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.HOOK$$Object", "ti.bios");
        po.init("ti.bios.HOOK.Object", om.findStrict("ti.bios.HOOK.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.HOOK$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void GBL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/GBL.xs");
        om.bind("ti.bios.GBL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.GBL.Module", "ti.bios");
        po.init("ti.bios.GBL.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("BOARDNAME", $$T_Str, $$UNDEF, "wh");
        po.addFld("PROCID", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("CLKIN", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("CLKOUT", Proto.Elm.newCNum("(xdc_Float)"), 0L, "wh");
        po.addFld("SPECIFYRTSLIB", $$T_Bool, $$UNDEF, "wh");
        po.addFld("RTSLIB", $$T_Str, $$UNDEF, "wh");
        po.addFld("ENDIANMODE", $$T_Str, $$UNDEF, "wh");
        po.addFld("CALLUSERINITFXN", $$T_Bool, 0L, "wh");
        po.addFld("CDBRELATIVEPATH", $$T_Str, $$UNDEF, "wh");
        po.addFld("USERINITFXN", $$T_Str, "FXN_F_nop", "wh");
        po.addFld("ENABLEINST", $$T_Bool, $$UNDEF, "wh");
        po.addFld("INSTRUMENTED", $$T_Bool, $$UNDEF, "wh");
        po.addFld("ENABLEALLTRC", $$T_Bool, true, "wh");
        po.addFld("C64PLUSCONFIGURE", $$T_Bool, 0L, "wh");
        po.addFld("C64PLUSL1PCFG", $$T_Str, $$UNDEF, "wh");
        po.addFld("C64PLUSL1DCFG", $$T_Str, $$UNDEF, "wh");
        po.addFld("C64PLUSL2CFG", $$T_Str, $$UNDEF, "wh");
        po.addFld("C64PLUSMAR0to31", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR32to63", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR64to95", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR96to127", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR128to159", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR160to191", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR192to223", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C64PLUSMAR224to255", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("C641XCONFIGUREL2", $$T_Bool, $$UNDEF, "wh");
        po.addFld("C641XCCFGL2MODE", $$T_Str, $$UNDEF, "wh");
        po.addFld("C641XMAREMIFB", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XMARCE0", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XMARCE1", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XMARCE2", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XMARCE3", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XCCFGP", $$T_Str, $$UNDEF, "wh");
        po.addFld("C641XSETL2ALLOC", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XL2ALLOC0", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XL2ALLOC1", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XL2ALLOC2", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("C641XL2ALLOC3", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("CSRPCC", $$T_Str, $$UNDEF, "wh");
        po.addFld("C621XCONFIGUREL2", $$T_Bool, $$UNDEF, "wh");
        po.addFld("C621XCCFGL2MODE", $$T_Str, $$UNDEF, "wh");
        po.addFld("C621XMAR", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("PLLTYPE", $$T_Str, $$UNDEF, "wh");
        po.addFld("MODIFYPLLCR0", $$T_Bool, $$UNDEF, "wh");
        po.addFld("PLLCR0", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("USERLIMPMODEABORTFXN", $$T_Str, $$UNDEF, "wh");
        po.addFld("MODIFYPLLCR1", $$T_Bool, $$UNDEF, "wh");
        po.addFld("PLLCR1", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("PLLWAITCYCLE", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("LOWERPMST", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("SWWSR", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("MODIFYSWCR", $$T_Bool, $$UNDEF, "wh");
        po.addFld("SWCR", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("BSCR", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("MODIFYCLKMD", $$T_Bool, $$UNDEF, "wh");
        po.addFld("CLKMD", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("CALLMODEL", $$T_Str, $$UNDEF, "wh");
        po.addFld("MEMORYMODEL", $$T_Str, $$UNDEF, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.GBL$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.GBL$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.GBL$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.GBL$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.GBL$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
    }

    void HST$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/HST.xs");
        om.bind("ti.bios.HST$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.HST.Module", "ti.bios");
        po.init("ti.bios.HST.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("HOSTLINKTYPE", $$T_Str, $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.HST$$create", "ti.bios"), Global.get("ti$bios$HST$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.HST$$construct", "ti.bios"), Global.get("ti$bios$HST$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.HST$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.HST$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.HST$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.HST$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.HST$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.HST.Instance", "ti.bios");
        po.init("ti.bios.HST.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("mode", $$T_Str, $$UNDEF, "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("bufAlign", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("frameSize", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("numFrames", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("statistics", $$T_Bool, $$UNDEF, "wh");
        po.addFld("availableForDHL", $$T_Bool, $$UNDEF, "wh");
        po.addFld("notifyFxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.HST$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.HST$$Params", "ti.bios");
        po.init("ti.bios.HST.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("mode", $$T_Str, $$UNDEF, "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("bufAlign", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("frameSize", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("numFrames", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
        po.addFld("statistics", $$T_Bool, $$UNDEF, "wh");
        po.addFld("availableForDHL", $$T_Bool, $$UNDEF, "wh");
        po.addFld("notifyFxn", $$T_Str, $$UNDEF, "wh");
        po.addFld("arg0", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
        po.addFld("arg1", new Proto.Adr("xdc_UArg", "Pv"), $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.HST$$Object", "ti.bios");
        po.init("ti.bios.HST.Object", om.findStrict("ti.bios.HST.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.HST$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void PIP$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/PIP.xs");
        om.bind("ti.bios.PIP$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.PIP.Module", "ti.bios");
        po.init("ti.bios.PIP.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.PIP$$create", "ti.bios"), Global.get("ti$bios$PIP$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.PIP$$construct", "ti.bios"), Global.get("ti$bios$PIP$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.PIP$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.PIP$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.PIP$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.PIP$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.PIP$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.PIP.Instance", "ti.bios");
        po.init("ti.bios.PIP.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufAlign", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "wh");
        po.addFld("frameSize", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("numFrames", Proto.Elm.newCNum("(xdc_UInt)"), 2L, "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("notifyWriterFxn", $$T_Str, null, "wh");
        po.addFld("notifyWriterArg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyWriterArg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyReaderFxn", $$T_Str, null, "wh");
        po.addFld("notifyReaderArg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyReaderArg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.PIP$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.PIP$$Params", "ti.bios");
        po.init("ti.bios.PIP.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("bufSeg", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), null, "wh");
        po.addFld("bufAlign", Proto.Elm.newCNum("(xdc_UInt)"), 1L, "wh");
        po.addFld("frameSize", Proto.Elm.newCNum("(xdc_UInt)"), 0L, "wh");
        po.addFld("numFrames", Proto.Elm.newCNum("(xdc_UInt)"), 2L, "wh");
        po.addFld("monitor", $$T_Str, $$UNDEF, "wh");
        po.addFld("notifyWriterFxn", $$T_Str, null, "wh");
        po.addFld("notifyWriterArg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyWriterArg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyReaderFxn", $$T_Str, null, "wh");
        po.addFld("notifyReaderArg0", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("notifyReaderArg1", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.PIP$$Object", "ti.bios");
        po.init("ti.bios.PIP.Object", om.findStrict("ti.bios.PIP.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.PIP$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void RTDX$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/RTDX.xs");
        om.bind("ti.bios.RTDX$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.RTDX.Module", "ti.bios");
        po.init("ti.bios.RTDX.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLERTDX", $$T_Bool, $$UNDEF, "wh");
        po.addFld("MODE", $$T_Str, $$UNDEF, "wh");
        po.addFld("RTDXDATASEG", (Proto)om.findStrict("ti.bios.MEM.Instance", "ti.bios"), $$UNDEF, "wh");
        po.addFld("BUFSIZE", Proto.Elm.newCNum("(xdc_UInt16)"), $$UNDEF, "wh");
        po.addFld("INTERRUPTMASK", Proto.Elm.newCNum("(xdc_UInt)"), $$UNDEF, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.RTDX$$create", "ti.bios"), Global.get("ti$bios$RTDX$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.RTDX$$construct", "ti.bios"), Global.get("ti$bios$RTDX$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.RTDX$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.RTDX$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.RTDX$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.RTDX$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.RTDX$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.RTDX.Instance", "ti.bios");
        po.init("ti.bios.RTDX.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("channelMode", $$T_Str, $$UNDEF, "wh");
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.RTDX$$destroy", "ti.bios"), Global.get(cap, "destroy"));
        po = (Proto.Obj)om.findStrict("ti.bios.RTDX$$Params", "ti.bios");
        po.init("ti.bios.RTDX.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("channelMode", $$T_Str, $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.RTDX$$Object", "ti.bios");
        po.init("ti.bios.RTDX.Object", om.findStrict("ti.bios.RTDX.Instance", "ti.bios"));
                po.addFxn("destroy", (Proto.Fxn)om.findStrict("ti.bios.RTDX$$destroy", "ti.bios"), Global.get(cap, "destroy"));
    }

    void ECM$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/ECM.xs");
        om.bind("ti.bios.ECM$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.ECM.Module", "ti.bios");
        po.init("ti.bios.ECM.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLE", $$T_Bool, 0L, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.ECM$$create", "ti.bios"), Global.get("ti$bios$ECM$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.ECM$$construct", "ti.bios"), Global.get("ti$bios$ECM$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.ECM$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.ECM$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.ECM$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.ECM$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.ECM$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("exportMod", (Proto.Fxn)om.findStrict("ti.bios.ECM$$exportMod", "ti.bios"), Global.get(cap, "exportMod"));
        po = (Proto.Obj)om.findStrict("ti.bios.ECM.Instance", "ti.bios");
        po.init("ti.bios.ECM.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("unmask", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.ECM$$Params", "ti.bios");
        po.init("ti.bios.ECM.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("unmask", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.ECM$$Object", "ti.bios");
        po.init("ti.bios.ECM.Object", om.findStrict("ti.bios.ECM.Instance", "ti.bios"));
    }

    void DEV$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/DEV.xs");
        om.bind("ti.bios.DEV$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.DEV.Module", "ti.bios");
        po.init("ti.bios.DEV.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.DEV$$create", "ti.bios"), Global.get("ti$bios$DEV$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.DEV$$construct", "ti.bios"), Global.get("ti$bios$DEV$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.DEV$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.DEV$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.DEV$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.DEV$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.DEV$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.DEV.Instance", "ti.bios");
        po.init("ti.bios.DEV.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("fxnTable", $$T_Str, null, "wh");
        po.addFld("fxnTableType", $$T_Str, "DEV_Fxns", "wh");
        po.addFld("deviceId", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("params", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("paramsType", $$T_Str, "Ptr", "wh");
        po.addFld("includePath", $$T_Str, null, "wh");
        po.addFld("deviceGlobalDataPtr", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DEV$$Params", "ti.bios");
        po.init("ti.bios.DEV.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("fxnTable", $$T_Str, null, "wh");
        po.addFld("fxnTableType", $$T_Str, "DEV_Fxns", "wh");
        po.addFld("deviceId", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("params", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("paramsType", $$T_Str, "Ptr", "wh");
        po.addFld("includePath", $$T_Str, null, "wh");
        po.addFld("deviceGlobalDataPtr", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DEV$$Object", "ti.bios");
        po.init("ti.bios.DEV.Object", om.findStrict("ti.bios.DEV.Instance", "ti.bios"));
    }

    void DGN$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/DGN.xs");
        om.bind("ti.bios.DGN$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.DGN.Module", "ti.bios");
        po.init("ti.bios.DGN.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.DGN$$create", "ti.bios"), Global.get("ti$bios$DGN$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.DGN$$construct", "ti.bios"), Global.get("ti$bios$DGN$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.DGN$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.DGN$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.DGN$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.DGN$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.DGN$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.DGN.Instance", "ti.bios");
        po.init("ti.bios.DGN.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("device", $$T_Str, "user", "wh");
        po.addFld("useDefaultParam", $$T_Bool, 0L, "wh");
        po.addFld("constant", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("seedValue", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("lowerLimit", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-32767"), "wh");
        po.addFld("upperLimit", Proto.Elm.newCNum("(xdc_Int)"), 32767L, "wh");
        po.addFld("gain", Proto.Elm.newCNum("(xdc_Int)"), 32767L, "wh");
        po.addFld("frequency", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("phase", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("rate", Proto.Elm.newCNum("(xdc_Int)"), 256L, "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("paramsType", $$T_Str, null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DGN$$Params", "ti.bios");
        po.init("ti.bios.DGN.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("device", $$T_Str, "user", "wh");
        po.addFld("useDefaultParam", $$T_Bool, 0L, "wh");
        po.addFld("constant", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("seedValue", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("lowerLimit", Proto.Elm.newCNum("(xdc_Int)"), Global.eval("-32767"), "wh");
        po.addFld("upperLimit", Proto.Elm.newCNum("(xdc_Int)"), 32767L, "wh");
        po.addFld("gain", Proto.Elm.newCNum("(xdc_Int)"), 32767L, "wh");
        po.addFld("frequency", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("phase", Proto.Elm.newCNum("(xdc_Int)"), 0L, "wh");
        po.addFld("rate", Proto.Elm.newCNum("(xdc_Int)"), 256L, "wh");
        po.addFld("fxn", $$T_Str, null, "wh");
        po.addFld("arg", new Proto.Adr("xdc_UArg", "Pv"), 0L, "wh");
        po.addFld("paramsType", $$T_Str, null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DGN$$Object", "ti.bios");
        po.init("ti.bios.DGN.Object", om.findStrict("ti.bios.DGN.Instance", "ti.bios"));
    }

    void DHL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/DHL.xs");
        om.bind("ti.bios.DHL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.DHL.Module", "ti.bios");
        po.init("ti.bios.DHL.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("OBJMEMSEG", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.DHL$$create", "ti.bios"), Global.get("ti$bios$DHL$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.DHL$$construct", "ti.bios"), Global.get("ti$bios$DHL$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.DHL$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.DHL$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.DHL$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.DHL$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.DHL$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.DHL.Instance", "ti.bios");
        po.init("ti.bios.DHL.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, $$UNDEF, "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("hstChannel", $$T_Str, $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DHL$$Params", "ti.bios");
        po.init("ti.bios.DHL.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, $$UNDEF, "wh");
        po.addFld("comment", $$T_Str, $$UNDEF, "wh");
        po.addFld("hstChannel", $$T_Str, $$UNDEF, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DHL$$Object", "ti.bios");
        po.init("ti.bios.DHL.Object", om.findStrict("ti.bios.DHL.Instance", "ti.bios"));
    }

    void DPI$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/DPI.xs");
        om.bind("ti.bios.DPI$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.DPI.Module", "ti.bios");
        po.init("ti.bios.DPI.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.DPI$$create", "ti.bios"), Global.get("ti$bios$DPI$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.DPI$$construct", "ti.bios"), Global.get("ti$bios$DPI$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.DPI$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.DPI$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.DPI$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.DPI$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.DPI$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.DPI.Instance", "ti.bios");
        po.init("ti.bios.DPI.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("allowVirtual", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DPI$$Params", "ti.bios");
        po.init("ti.bios.DPI.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, "<add comments here>", "wh");
        po.addFld("allowVirtual", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DPI$$Object", "ti.bios");
        po.init("ti.bios.DPI.Object", om.findStrict("ti.bios.DPI.Instance", "ti.bios"));
    }

    void DIO$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/DIO.xs");
        om.bind("ti.bios.DIO$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.DIO.Module", "ti.bios");
        po.init("ti.bios.DIO.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("STATICCREATE", $$T_Bool, false, "wh");
        po.addFld("OBJMEMSEG", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.DIO$$create", "ti.bios"), Global.get("ti$bios$DIO$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.DIO$$construct", "ti.bios"), Global.get("ti$bios$DIO$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.DIO$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.DIO$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.DIO$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.DIO$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.DIO$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.DIO.Instance", "ti.bios");
        po.init("ti.bios.DIO.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, null, "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("useCallBackFxn", $$T_Bool, false, "wh");
        po.addFld("deviceName", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("chanParams", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DIO$$Params", "ti.bios");
        po.init("ti.bios.DIO.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, null, "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("useCallBackFxn", $$T_Bool, false, "wh");
        po.addFld("deviceName", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("chanParams", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.DIO$$Object", "ti.bios");
        po.init("ti.bios.DIO.Object", om.findStrict("ti.bios.DIO.Instance", "ti.bios"));
    }

    void GIO$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/GIO.xs");
        om.bind("ti.bios.GIO$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.GIO.Module", "ti.bios");
        po.init("ti.bios.GIO.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLEGIO", $$T_Bool, false, "wh");
        po.addFld("CREATEFXN", $$T_Str, "SEM_create", "wh");
        po.addFld("DELETEFXN", $$T_Str, "SEM_delete", "wh");
        po.addFld("PENDFXN", $$T_Str, "SEM_pend", "wh");
        po.addFld("POSTFXN", $$T_Str, "SEM_post", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.GIO$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.GIO$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.GIO$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.GIO$$close", "ti.bios"), Global.get(cap, "close"));
    }

    void SIO$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/SIO.xs");
        om.bind("ti.bios.SIO$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.SIO.Module", "ti.bios");
        po.init("ti.bios.SIO.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("USEISSUERECLAIM", $$T_Bool, false, "wh");
        po.addFld("OBJMEMSEG", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.SIO$$create", "ti.bios"), Global.get("ti$bios$SIO$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.SIO$$construct", "ti.bios"), Global.get("ti$bios$SIO$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.SIO$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.SIO$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.SIO$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.SIO$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.SIO$$close", "ti.bios"), Global.get(cap, "close"));
        po = (Proto.Obj)om.findStrict("ti.bios.SIO.Instance", "ti.bios");
        po.init("ti.bios.SIO.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, null, "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("deviceName", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("controlParameter", $$T_Str, null, "wh");
        po.addFld("mode", $$T_Str, null, "wh");
        po.addFld("bufSize", $$T_Str, null, "wh");
        po.addFld("numBufs", $$T_Str, null, "wh");
        po.addFld("flush", $$T_Str, null, "wh");
        po.addFld("bufAlign", $$T_Str, null, "wh");
        po.addFld("bufSegId", $$T_Str, null, "wh");
        po.addFld("modelName", $$T_Str, null, "wh");
        po.addFld("allocStaticBuf", $$T_Str, null, "wh");
        po.addFld("timeout", $$T_Str, null, "wh");
        po.addFld("useCallBackFxn", $$T_Str, null, "wh");
        po.addFld("callBackFxn", $$T_Str, null, "wh");
        po.addFld("arg0", $$T_Str, null, "wh");
        po.addFld("arg1", $$T_Str, null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.SIO$$Params", "ti.bios");
        po.init("ti.bios.SIO.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, null, "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("deviceName", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("controlParameter", $$T_Str, null, "wh");
        po.addFld("mode", $$T_Str, null, "wh");
        po.addFld("bufSize", $$T_Str, null, "wh");
        po.addFld("numBufs", $$T_Str, null, "wh");
        po.addFld("flush", $$T_Str, null, "wh");
        po.addFld("bufAlign", $$T_Str, null, "wh");
        po.addFld("bufSegId", $$T_Str, null, "wh");
        po.addFld("modelName", $$T_Str, null, "wh");
        po.addFld("allocStaticBuf", $$T_Str, null, "wh");
        po.addFld("timeout", $$T_Str, null, "wh");
        po.addFld("useCallBackFxn", $$T_Str, null, "wh");
        po.addFld("callBackFxn", $$T_Str, null, "wh");
        po.addFld("arg0", $$T_Str, null, "wh");
        po.addFld("arg1", $$T_Str, null, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.SIO$$Object", "ti.bios");
        po.init("ti.bios.SIO.Object", om.findStrict("ti.bios.SIO.Instance", "ti.bios"));
    }

    void UDEV$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/UDEV.xs");
        om.bind("ti.bios.UDEV$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.UDEV.Module", "ti.bios");
        po.init("ti.bios.UDEV.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFxn("create", (Proto.Fxn)om.findStrict("ti.bios.UDEV$$create", "ti.bios"), Global.get("ti$bios$UDEV$$create"));
                po.addFxn("construct", (Proto.Fxn)om.findStrict("ti.bios.UDEV$$construct", "ti.bios"), Global.get("ti$bios$UDEV$$construct"));
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.UDEV$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.UDEV$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "instance$meta$init");
        if (fxn != null) om.bind("ti.bios.UDEV$$instance$meta$init", true);
        if (fxn != null) po.addFxn("instance$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.UDEV$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.UDEV$$close", "ti.bios"), Global.get(cap, "close"));
                po.addFxn("get", (Proto.Fxn)om.findStrict("ti.bios.UDEV$$get", "ti.bios"), Global.get(cap, "get"));
        po = (Proto.Obj)om.findStrict("ti.bios.UDEV.Instance", "ti.bios");
        po.init("ti.bios.UDEV.Instance", $$Instance);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("fxnTable", $$T_Str, null, "wh");
        po.addFld("fxnTableType", $$T_Str, "DEV_Fxns", "wh");
        po.addFld("deviceId", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("params", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("deviceGlobalDataPtr", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "wh");
        po.addFld("stackingDevice", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.UDEV$$Params", "ti.bios");
        po.init("ti.bios.UDEV.Params", $$Params);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "", "wh");
        po.addFld("comment", $$T_Str, null, "wh");
        po.addFld("initFxn", $$T_Str, null, "wh");
        po.addFld("fxnTable", $$T_Str, null, "wh");
        po.addFld("fxnTableType", $$T_Str, "DEV_Fxns", "wh");
        po.addFld("deviceId", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("params", new Proto.Adr("xdc_Ptr", "Pv"), null, "wh");
        po.addFld("deviceGlobalDataPtr", new Proto.Adr("xdc_Ptr", "Pv"), 0L, "wh");
        po.addFld("stackingDevice", $$T_Bool, false, "wh");
        po = (Proto.Obj)om.findStrict("ti.bios.UDEV$$Object", "ti.bios");
        po.init("ti.bios.UDEV.Object", om.findStrict("ti.bios.UDEV.Instance", "ti.bios"));
    }

    void MSGQ$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/MSGQ.xs");
        om.bind("ti.bios.MSGQ$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.MSGQ.Module", "ti.bios");
        po.init("ti.bios.MSGQ.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLEMSGQ", $$T_Bool, false, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.MSGQ$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.MSGQ$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.MSGQ$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.MSGQ$$close", "ti.bios"), Global.get(cap, "close"));
    }

    void POOL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/POOL.xs");
        om.bind("ti.bios.POOL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.POOL.Module", "ti.bios");
        po.init("ti.bios.POOL.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("ENABLEPOOL", $$T_Bool, false, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.POOL$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.POOL$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.POOL$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.POOL$$close", "ti.bios"), Global.get(cap, "close"));
    }

    void STATICPOOL$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/STATICPOOL.xs");
        om.bind("ti.bios.STATICPOOL$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.bios.STATICPOOL.Module", "ti.bios");
        po.init("ti.bios.STATICPOOL.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.bios.STATICPOOL$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.bios.STATICPOOL$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.bios.STATICPOOL$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("close", (Proto.Fxn)om.findStrict("ti.bios.STATICPOOL$$close", "ti.bios"), Global.get(cap, "close"));
    }

    void SYS$$ROV()
    {
    }

    void LOG$$ROV()
    {
    }

    void MEM$$ROV()
    {
    }

    void TSK$$ROV()
    {
    }

    void CLK$$ROV()
    {
    }

    void SEM$$ROV()
    {
    }

    void SWI$$ROV()
    {
    }

    void IDL$$ROV()
    {
    }

    void MBX$$ROV()
    {
    }

    void STS$$ROV()
    {
    }

    void QUE$$ROV()
    {
    }

    void PRD$$ROV()
    {
    }

    void HWI$$ROV()
    {
    }

    void PIE$$ROV()
    {
    }

    void BUF$$ROV()
    {
    }

    void LCK$$ROV()
    {
    }

    void HOOK$$ROV()
    {
    }

    void GBL$$ROV()
    {
    }

    void HST$$ROV()
    {
    }

    void PIP$$ROV()
    {
    }

    void RTDX$$ROV()
    {
    }

    void ECM$$ROV()
    {
    }

    void DEV$$ROV()
    {
    }

    void DGN$$ROV()
    {
    }

    void DHL$$ROV()
    {
    }

    void DPI$$ROV()
    {
    }

    void DIO$$ROV()
    {
    }

    void GIO$$ROV()
    {
    }

    void SIO$$ROV()
    {
    }

    void UDEV$$ROV()
    {
    }

    void MSGQ$$ROV()
    {
    }

    void POOL$$ROV()
    {
    }

    void STATICPOOL$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.bios.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.bios"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/bios/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.bios"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.bios"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.bios"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.bios"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.bios"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.bios"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.bios", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.bios");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.bios.");
        pkgV.bind("$vers", Global.newArray("5, 6, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.knl", Global.newArray()));
        imports.add(Global.newArray("ti.sysbios.misc", Global.newArray()));
        imports.add(Global.newArray("ti.sysbios.heaps", Global.newArray()));
        imports.add(Global.newArray("ti.sysbios.knl", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.bios'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/release/ti.bios.a28L',\n");
            sb.append("'lib/release/ti.bios.a28FP',\n");
            sb.append("'lib/release/ti.bios.a64P',\n");
            sb.append("'lib/release/ti.bios.a64Pe',\n");
            sb.append("'lib/release/ti.bios.a674',\n");
            sb.append("'lib/release/ti.bios.ae64P',\n");
            sb.append("'lib/release/ti.bios.ae64Pe',\n");
            sb.append("'lib/release/ti.bios.ae674',\n");
            sb.append("'lib/release/ti.bios.ae64T',\n");
            sb.append("'lib/release/ti.bios.ae66',\n");
            sb.append("'lib/release/ti.bios.ae66e',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/release/ti.bios.a28L', {target: 'ti.targets.C28_large', suffix: '28L'}],\n");
            sb.append("['lib/release/ti.bios.a28FP', {target: 'ti.targets.C28_float', suffix: '28FP'}],\n");
            sb.append("['lib/release/ti.bios.a64P', {target: 'ti.targets.C64P', suffix: '64P'}],\n");
            sb.append("['lib/release/ti.bios.a64Pe', {target: 'ti.targets.C64P_big_endian', suffix: '64Pe'}],\n");
            sb.append("['lib/release/ti.bios.a674', {target: 'ti.targets.C674', suffix: '674'}],\n");
            sb.append("['lib/release/ti.bios.ae64P', {target: 'ti.targets.elf.C64P', suffix: 'e64P'}],\n");
            sb.append("['lib/release/ti.bios.ae64Pe', {target: 'ti.targets.elf.C64P_big_endian', suffix: 'e64Pe'}],\n");
            sb.append("['lib/release/ti.bios.ae674', {target: 'ti.targets.elf.C674', suffix: 'e674'}],\n");
            sb.append("['lib/release/ti.bios.ae64T', {target: 'ti.targets.elf.C64T', suffix: 'e64T'}],\n");
            sb.append("['lib/release/ti.bios.ae66', {target: 'ti.targets.elf.C66', suffix: 'e66'}],\n");
            sb.append("['lib/release/ti.bios.ae66e', {target: 'ti.targets.elf.C66_big_endian', suffix: 'e66e'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void SYS$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.SYS", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.SYS.Module", "ti.bios");
        vo.init2(po, "ti.bios.SYS", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.SYS$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.SYS$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("SYS", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SYS");
    }

    void LOG$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.LOG", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.LOG.Module", "ti.bios");
        vo.init2(po, "ti.bios.LOG", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.LOG$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.LOG.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.LOG.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.LOG.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.LOG$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.LOG.Object", "ti.bios"));
        pkgV.bind("LOG", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LOG");
    }

    void MEM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.MEM", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.MEM.Module", "ti.bios");
        vo.init2(po, "ti.bios.MEM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.MEM$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.MEM.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.MEM.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.MEM.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("HeapList", om.findStrict("ti.bios.MEM.HeapList", "ti.bios"));
        tdefs.add(om.findStrict("ti.bios.MEM.HeapList", "ti.bios"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.MEM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./MEM.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.MEM.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./MEM.xdt");
        pkgV.bind("MEM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MEM");
    }

    void TSK$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.TSK", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.TSK.Module", "ti.bios");
        vo.init2(po, "ti.bios.TSK", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.TSK$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.TSK.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.TSK.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.TSK.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.TSK$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./TSK.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.TSK.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./TSK.xdt");
        pkgV.bind("TSK", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TSK");
    }

    void CLK$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.CLK", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.CLK.Module", "ti.bios");
        vo.init2(po, "ti.bios.CLK", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.CLK$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.CLK.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.CLK.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.CLK.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.CLK$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./CLK.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.CLK.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./CLK.xdt");
        pkgV.bind("CLK", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("CLK");
    }

    void SEM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.SEM", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.SEM.Module", "ti.bios");
        vo.init2(po, "ti.bios.SEM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.SEM$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.SEM.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.SEM.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.SEM.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.SEM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.SEM.Object", "ti.bios"));
        pkgV.bind("SEM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SEM");
    }

    void SWI$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.SWI", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.SWI.Module", "ti.bios");
        vo.init2(po, "ti.bios.SWI", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.SWI$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.SWI.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.SWI.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.SWI.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.SWI$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./SWI.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.SWI.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./SWI.xdt");
        pkgV.bind("SWI", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SWI");
    }

    void IDL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.IDL", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.IDL.Module", "ti.bios");
        vo.init2(po, "ti.bios.IDL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.IDL$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.IDL.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.IDL.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.IDL.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.IDL$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.IDL.Object", "ti.bios"));
        pkgV.bind("IDL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("IDL");
    }

    void MBX$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.MBX", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.MBX.Module", "ti.bios");
        vo.init2(po, "ti.bios.MBX", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.MBX$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.MBX.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.MBX.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.MBX.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.MBX$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.MBX.Object", "ti.bios"));
        pkgV.bind("MBX", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MBX");
    }

    void STS$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.STS", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.STS.Module", "ti.bios");
        vo.init2(po, "ti.bios.STS", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.STS$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.STS.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.STS.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.STS.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.STS$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.STS.Object", "ti.bios"));
        pkgV.bind("STS", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("STS");
    }

    void QUE$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.QUE", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.QUE.Module", "ti.bios");
        vo.init2(po, "ti.bios.QUE", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.QUE$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.QUE.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.QUE.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.QUE.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.QUE$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./QUE.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.QUE.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./QUE.xdt");
        pkgV.bind("QUE", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("QUE");
    }

    void PRD$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.PRD", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.PRD.Module", "ti.bios");
        vo.init2(po, "ti.bios.PRD", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.PRD$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.PRD.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.PRD.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.PRD.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.PRD$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./PRD.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.PRD.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./PRD.xdt");
        pkgV.bind("PRD", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("PRD");
    }

    void HWI$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.HWI", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.HWI.Module", "ti.bios");
        vo.init2(po, "ti.bios.HWI", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.HWI$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.HWI.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.HWI.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.HWI.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.HWI$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./HWI.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.HWI.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./HWI.xdt");
        pkgV.bind("HWI", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HWI");
    }

    void PIE$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.PIE", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.PIE.Module", "ti.bios");
        vo.init2(po, "ti.bios.PIE", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.PIE$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.PIE.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.PIE.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.PIE.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.PIE$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./PIE.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.PIE.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./PIE.xdt");
        pkgV.bind("PIE", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("PIE");
    }

    void BUF$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.BUF", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.BUF.Module", "ti.bios");
        vo.init2(po, "ti.bios.BUF", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.BUF$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.BUF.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.BUF.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.BUF.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.BUF$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.BUF.Object", "ti.bios"));
        pkgV.bind("BUF", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("BUF");
    }

    void LCK$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.LCK", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.LCK.Module", "ti.bios");
        vo.init2(po, "ti.bios.LCK", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.LCK$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.LCK.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.LCK.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.LCK.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.LCK$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.LCK.Object", "ti.bios"));
        pkgV.bind("LCK", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("LCK");
    }

    void HOOK$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.HOOK", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.HOOK.Module", "ti.bios");
        vo.init2(po, "ti.bios.HOOK", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.HOOK$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.HOOK.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.HOOK.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.HOOK.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.HOOK$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.HOOK.Object", "ti.bios"));
        pkgV.bind("HOOK", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HOOK");
    }

    void GBL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.GBL", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.GBL.Module", "ti.bios");
        vo.init2(po, "ti.bios.GBL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.GBL$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.GBL$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./GBL.xdt");
        atmap.seal("length");
        vo.bind("TEMPLATE$", "./GBL.xdt");
        pkgV.bind("GBL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GBL");
    }

    void HST$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.HST", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.HST.Module", "ti.bios");
        vo.init2(po, "ti.bios.HST", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.HST$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.HST.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.HST.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.HST.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.HST$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.HST.Object", "ti.bios"));
        pkgV.bind("HST", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("HST");
    }

    void PIP$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.PIP", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.PIP.Module", "ti.bios");
        vo.init2(po, "ti.bios.PIP", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.PIP$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.PIP.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.PIP.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.PIP.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.PIP$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./PIP.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.PIP.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./PIP.xdt");
        pkgV.bind("PIP", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("PIP");
    }

    void RTDX$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.RTDX", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.RTDX.Module", "ti.bios");
        vo.init2(po, "ti.bios.RTDX", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.RTDX$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.RTDX.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.RTDX.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.RTDX.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.RTDX$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.RTDX.Object", "ti.bios"));
        pkgV.bind("RTDX", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("RTDX");
    }

    void ECM$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.ECM", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.ECM.Module", "ti.bios");
        vo.init2(po, "ti.bios.ECM", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.ECM$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.ECM.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.ECM.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.ECM.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.ECM$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.ECM.Object", "ti.bios"));
        pkgV.bind("ECM", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ECM");
    }

    void DEV$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.DEV", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.DEV.Module", "ti.bios");
        vo.init2(po, "ti.bios.DEV", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.DEV$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.DEV.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.DEV.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.DEV.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.DEV$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./DEV.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.DEV.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./DEV.xdt");
        pkgV.bind("DEV", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DEV");
    }

    void DGN$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.DGN", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.DGN.Module", "ti.bios");
        vo.init2(po, "ti.bios.DGN", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.DGN$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.DGN.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.DGN.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.DGN.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.DGN$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./DGN.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.DGN.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./DGN.xdt");
        pkgV.bind("DGN", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DGN");
    }

    void DHL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.DHL", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.DHL.Module", "ti.bios");
        vo.init2(po, "ti.bios.DHL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.DHL$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.DHL.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.DHL.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.DHL.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.DHL$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.DHL.Object", "ti.bios"));
        pkgV.bind("DHL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DHL");
    }

    void DPI$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.DPI", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.DPI.Module", "ti.bios");
        vo.init2(po, "ti.bios.DPI", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.DPI$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.DPI.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.DPI.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.DPI.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.DPI$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.DPI.Object", "ti.bios"));
        pkgV.bind("DPI", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DPI");
    }

    void DIO$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.DIO", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.DIO.Module", "ti.bios");
        vo.init2(po, "ti.bios.DIO", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.DIO$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.DIO.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.DIO.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.DIO.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.DIO$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./DIO.xdt");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.DIO.Object", "ti.bios"));
        vo.bind("TEMPLATE$", "./DIO.xdt");
        pkgV.bind("DIO", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("DIO");
    }

    void GIO$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.GIO", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.GIO.Module", "ti.bios");
        vo.init2(po, "ti.bios.GIO", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.GIO$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.GIO$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "./GIO.xdt");
        atmap.seal("length");
        vo.bind("TEMPLATE$", "./GIO.xdt");
        pkgV.bind("GIO", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("GIO");
    }

    void SIO$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.SIO", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.SIO.Module", "ti.bios");
        vo.init2(po, "ti.bios.SIO", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.SIO$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.SIO.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.SIO.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.SIO.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.SIO$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.SIO.Object", "ti.bios"));
        pkgV.bind("SIO", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SIO");
    }

    void UDEV$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.UDEV", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.UDEV.Module", "ti.bios");
        vo.init2(po, "ti.bios.UDEV", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.UDEV$$capsule", "ti.bios"));
        vo.bind("Instance", om.findStrict("ti.bios.UDEV.Instance", "ti.bios"));
        vo.bind("Params", om.findStrict("ti.bios.UDEV.Params", "ti.bios"));
        vo.bind("PARAMS", ((Proto.Str)om.findStrict("ti.bios.UDEV.Params", "ti.bios")).newInstance());
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 1);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.UDEV$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("Object", om.findStrict("ti.bios.UDEV.Object", "ti.bios"));
        pkgV.bind("UDEV", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("UDEV");
    }

    void MSGQ$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.MSGQ", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.MSGQ.Module", "ti.bios");
        vo.init2(po, "ti.bios.MSGQ", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.MSGQ$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.MSGQ$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("MSGQ", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("MSGQ");
    }

    void POOL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.POOL", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.POOL.Module", "ti.bios");
        vo.init2(po, "ti.bios.POOL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.POOL$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.POOL$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("POOL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("POOL");
    }

    void STATICPOOL$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.bios.STATICPOOL", "ti.bios");
        po = (Proto.Obj)om.findStrict("ti.bios.STATICPOOL.Module", "ti.bios");
        vo.init2(po, "ti.bios.STATICPOOL", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.bios.STATICPOOL$$capsule", "ti.bios"));
        vo.bind("$package", om.findStrict("ti.bios", "ti.bios"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.bios")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.bios.STATICPOOL$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("STATICPOOL", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("STATICPOOL");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.SYS", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.LOG", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.MEM", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.TSK", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.CLK", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.SEM", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.SWI", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.IDL", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.MBX", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.STS", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.QUE", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.PRD", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.HWI", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.PIE", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.BUF", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.LCK", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.HOOK", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.GBL", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.HST", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.PIP", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.RTDX", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.ECM", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.DEV", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.DGN", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.DHL", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.DPI", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.DIO", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.GIO", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.SIO", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.UDEV", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.MSGQ", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.POOL", "ti.bios"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.bios.STATICPOOL", "ti.bios"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.bios.SYS")).bless();
        ((Value.Obj)om.getv("ti.bios.LOG")).bless();
        ((Value.Obj)om.getv("ti.bios.MEM")).bless();
        ((Value.Obj)om.getv("ti.bios.TSK")).bless();
        ((Value.Obj)om.getv("ti.bios.CLK")).bless();
        ((Value.Obj)om.getv("ti.bios.SEM")).bless();
        ((Value.Obj)om.getv("ti.bios.SWI")).bless();
        ((Value.Obj)om.getv("ti.bios.IDL")).bless();
        ((Value.Obj)om.getv("ti.bios.MBX")).bless();
        ((Value.Obj)om.getv("ti.bios.STS")).bless();
        ((Value.Obj)om.getv("ti.bios.QUE")).bless();
        ((Value.Obj)om.getv("ti.bios.PRD")).bless();
        ((Value.Obj)om.getv("ti.bios.HWI")).bless();
        ((Value.Obj)om.getv("ti.bios.PIE")).bless();
        ((Value.Obj)om.getv("ti.bios.BUF")).bless();
        ((Value.Obj)om.getv("ti.bios.LCK")).bless();
        ((Value.Obj)om.getv("ti.bios.HOOK")).bless();
        ((Value.Obj)om.getv("ti.bios.GBL")).bless();
        ((Value.Obj)om.getv("ti.bios.HST")).bless();
        ((Value.Obj)om.getv("ti.bios.PIP")).bless();
        ((Value.Obj)om.getv("ti.bios.RTDX")).bless();
        ((Value.Obj)om.getv("ti.bios.ECM")).bless();
        ((Value.Obj)om.getv("ti.bios.DEV")).bless();
        ((Value.Obj)om.getv("ti.bios.DGN")).bless();
        ((Value.Obj)om.getv("ti.bios.DHL")).bless();
        ((Value.Obj)om.getv("ti.bios.DPI")).bless();
        ((Value.Obj)om.getv("ti.bios.DIO")).bless();
        ((Value.Obj)om.getv("ti.bios.GIO")).bless();
        ((Value.Obj)om.getv("ti.bios.SIO")).bless();
        ((Value.Obj)om.getv("ti.bios.UDEV")).bless();
        ((Value.Obj)om.getv("ti.bios.MSGQ")).bless();
        ((Value.Obj)om.getv("ti.bios.POOL")).bless();
        ((Value.Obj)om.getv("ti.bios.STATICPOOL")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.bios")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        SYS$$OBJECTS();
        LOG$$OBJECTS();
        MEM$$OBJECTS();
        TSK$$OBJECTS();
        CLK$$OBJECTS();
        SEM$$OBJECTS();
        SWI$$OBJECTS();
        IDL$$OBJECTS();
        MBX$$OBJECTS();
        STS$$OBJECTS();
        QUE$$OBJECTS();
        PRD$$OBJECTS();
        HWI$$OBJECTS();
        PIE$$OBJECTS();
        BUF$$OBJECTS();
        LCK$$OBJECTS();
        HOOK$$OBJECTS();
        GBL$$OBJECTS();
        HST$$OBJECTS();
        PIP$$OBJECTS();
        RTDX$$OBJECTS();
        ECM$$OBJECTS();
        DEV$$OBJECTS();
        DGN$$OBJECTS();
        DHL$$OBJECTS();
        DPI$$OBJECTS();
        DIO$$OBJECTS();
        GIO$$OBJECTS();
        SIO$$OBJECTS();
        UDEV$$OBJECTS();
        MSGQ$$OBJECTS();
        POOL$$OBJECTS();
        STATICPOOL$$OBJECTS();
        SYS$$CONSTS();
        LOG$$CONSTS();
        MEM$$CONSTS();
        TSK$$CONSTS();
        CLK$$CONSTS();
        SEM$$CONSTS();
        SWI$$CONSTS();
        IDL$$CONSTS();
        MBX$$CONSTS();
        STS$$CONSTS();
        QUE$$CONSTS();
        PRD$$CONSTS();
        HWI$$CONSTS();
        PIE$$CONSTS();
        BUF$$CONSTS();
        LCK$$CONSTS();
        HOOK$$CONSTS();
        GBL$$CONSTS();
        HST$$CONSTS();
        PIP$$CONSTS();
        RTDX$$CONSTS();
        ECM$$CONSTS();
        DEV$$CONSTS();
        DGN$$CONSTS();
        DHL$$CONSTS();
        DPI$$CONSTS();
        DIO$$CONSTS();
        GIO$$CONSTS();
        SIO$$CONSTS();
        UDEV$$CONSTS();
        MSGQ$$CONSTS();
        POOL$$CONSTS();
        STATICPOOL$$CONSTS();
        SYS$$CREATES();
        LOG$$CREATES();
        MEM$$CREATES();
        TSK$$CREATES();
        CLK$$CREATES();
        SEM$$CREATES();
        SWI$$CREATES();
        IDL$$CREATES();
        MBX$$CREATES();
        STS$$CREATES();
        QUE$$CREATES();
        PRD$$CREATES();
        HWI$$CREATES();
        PIE$$CREATES();
        BUF$$CREATES();
        LCK$$CREATES();
        HOOK$$CREATES();
        GBL$$CREATES();
        HST$$CREATES();
        PIP$$CREATES();
        RTDX$$CREATES();
        ECM$$CREATES();
        DEV$$CREATES();
        DGN$$CREATES();
        DHL$$CREATES();
        DPI$$CREATES();
        DIO$$CREATES();
        GIO$$CREATES();
        SIO$$CREATES();
        UDEV$$CREATES();
        MSGQ$$CREATES();
        POOL$$CREATES();
        STATICPOOL$$CREATES();
        SYS$$FUNCTIONS();
        LOG$$FUNCTIONS();
        MEM$$FUNCTIONS();
        TSK$$FUNCTIONS();
        CLK$$FUNCTIONS();
        SEM$$FUNCTIONS();
        SWI$$FUNCTIONS();
        IDL$$FUNCTIONS();
        MBX$$FUNCTIONS();
        STS$$FUNCTIONS();
        QUE$$FUNCTIONS();
        PRD$$FUNCTIONS();
        HWI$$FUNCTIONS();
        PIE$$FUNCTIONS();
        BUF$$FUNCTIONS();
        LCK$$FUNCTIONS();
        HOOK$$FUNCTIONS();
        GBL$$FUNCTIONS();
        HST$$FUNCTIONS();
        PIP$$FUNCTIONS();
        RTDX$$FUNCTIONS();
        ECM$$FUNCTIONS();
        DEV$$FUNCTIONS();
        DGN$$FUNCTIONS();
        DHL$$FUNCTIONS();
        DPI$$FUNCTIONS();
        DIO$$FUNCTIONS();
        GIO$$FUNCTIONS();
        SIO$$FUNCTIONS();
        UDEV$$FUNCTIONS();
        MSGQ$$FUNCTIONS();
        POOL$$FUNCTIONS();
        STATICPOOL$$FUNCTIONS();
        SYS$$SIZES();
        LOG$$SIZES();
        MEM$$SIZES();
        TSK$$SIZES();
        CLK$$SIZES();
        SEM$$SIZES();
        SWI$$SIZES();
        IDL$$SIZES();
        MBX$$SIZES();
        STS$$SIZES();
        QUE$$SIZES();
        PRD$$SIZES();
        HWI$$SIZES();
        PIE$$SIZES();
        BUF$$SIZES();
        LCK$$SIZES();
        HOOK$$SIZES();
        GBL$$SIZES();
        HST$$SIZES();
        PIP$$SIZES();
        RTDX$$SIZES();
        ECM$$SIZES();
        DEV$$SIZES();
        DGN$$SIZES();
        DHL$$SIZES();
        DPI$$SIZES();
        DIO$$SIZES();
        GIO$$SIZES();
        SIO$$SIZES();
        UDEV$$SIZES();
        MSGQ$$SIZES();
        POOL$$SIZES();
        STATICPOOL$$SIZES();
        SYS$$TYPES();
        LOG$$TYPES();
        MEM$$TYPES();
        TSK$$TYPES();
        CLK$$TYPES();
        SEM$$TYPES();
        SWI$$TYPES();
        IDL$$TYPES();
        MBX$$TYPES();
        STS$$TYPES();
        QUE$$TYPES();
        PRD$$TYPES();
        HWI$$TYPES();
        PIE$$TYPES();
        BUF$$TYPES();
        LCK$$TYPES();
        HOOK$$TYPES();
        GBL$$TYPES();
        HST$$TYPES();
        PIP$$TYPES();
        RTDX$$TYPES();
        ECM$$TYPES();
        DEV$$TYPES();
        DGN$$TYPES();
        DHL$$TYPES();
        DPI$$TYPES();
        DIO$$TYPES();
        GIO$$TYPES();
        SIO$$TYPES();
        UDEV$$TYPES();
        MSGQ$$TYPES();
        POOL$$TYPES();
        STATICPOOL$$TYPES();
        if (isROV) {
            SYS$$ROV();
            LOG$$ROV();
            MEM$$ROV();
            TSK$$ROV();
            CLK$$ROV();
            SEM$$ROV();
            SWI$$ROV();
            IDL$$ROV();
            MBX$$ROV();
            STS$$ROV();
            QUE$$ROV();
            PRD$$ROV();
            HWI$$ROV();
            PIE$$ROV();
            BUF$$ROV();
            LCK$$ROV();
            HOOK$$ROV();
            GBL$$ROV();
            HST$$ROV();
            PIP$$ROV();
            RTDX$$ROV();
            ECM$$ROV();
            DEV$$ROV();
            DGN$$ROV();
            DHL$$ROV();
            DPI$$ROV();
            DIO$$ROV();
            GIO$$ROV();
            SIO$$ROV();
            UDEV$$ROV();
            MSGQ$$ROV();
            POOL$$ROV();
            STATICPOOL$$ROV();
        }//isROV
        $$SINGLETONS();
        SYS$$SINGLETONS();
        LOG$$SINGLETONS();
        MEM$$SINGLETONS();
        TSK$$SINGLETONS();
        CLK$$SINGLETONS();
        SEM$$SINGLETONS();
        SWI$$SINGLETONS();
        IDL$$SINGLETONS();
        MBX$$SINGLETONS();
        STS$$SINGLETONS();
        QUE$$SINGLETONS();
        PRD$$SINGLETONS();
        HWI$$SINGLETONS();
        PIE$$SINGLETONS();
        BUF$$SINGLETONS();
        LCK$$SINGLETONS();
        HOOK$$SINGLETONS();
        GBL$$SINGLETONS();
        HST$$SINGLETONS();
        PIP$$SINGLETONS();
        RTDX$$SINGLETONS();
        ECM$$SINGLETONS();
        DEV$$SINGLETONS();
        DGN$$SINGLETONS();
        DHL$$SINGLETONS();
        DPI$$SINGLETONS();
        DIO$$SINGLETONS();
        GIO$$SINGLETONS();
        SIO$$SINGLETONS();
        UDEV$$SINGLETONS();
        MSGQ$$SINGLETONS();
        POOL$$SINGLETONS();
        STATICPOOL$$SINGLETONS();
        $$INITIALIZATION();
    }
}
