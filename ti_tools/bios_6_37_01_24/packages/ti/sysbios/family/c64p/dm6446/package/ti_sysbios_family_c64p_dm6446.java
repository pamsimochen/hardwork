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

public class ti_sysbios_family_c64p_dm6446
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
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.interfaces");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.family.c64p.dm6446.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.family.c64p.dm6446", new Value.Obj("ti.sysbios.family.c64p.dm6446", pkgP));
    }

    void TimerSupport$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport", new Value.Obj("ti.sysbios.family.c64p.dm6446.TimerSupport", po));
        pkgV.bind("TimerSupport", vo);
        // decls 
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", new Proto.Enm("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc"));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$Emulation", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", new Proto.Str(spo, false));
        spo = (Proto.Obj)om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$Module_State", new Proto.Obj());
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", new Proto.Str(spo, false));
    }

    void TimerSupport$$CONSTS()
    {
        // module TimerSupport
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_ARM", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.dm6446"), "ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_ARM", xdc.services.intern.xsr.Enum.intValue(0L)+0));
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_DSP", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.dm6446"), "ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_DSP", xdc.services.intern.xsr.Enum.intValue(1L)+0));
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport.enable", new Extern("ti_sysbios_family_c64p_dm6446_TimerSupport_enable__E", "xdc_Void(*)(xdc_UInt,xdc_runtime_Error_Block*)", true, false));
    }

    void TimerSupport$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void TimerSupport$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void TimerSupport$$SIZES()
    {
        Proto.Str so;
        Object fxn;

        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", "ti.sysbios.family.c64p.dm6446");
        sizes.clear();
        sizes.add(Global.newArray("suspSrc", "Nti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc;;0;1"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation'], fld); }");
        so.bind("$offsetof", fxn);
        so = (Proto.Str)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", "ti.sysbios.family.c64p.dm6446");
        sizes.clear();
        sizes.add(Global.newArray("suspSrc", "UPtr"));
        so.bind("$$sizes", Global.newArray(sizes.toArray()));
        fxn = Global.eval("function() { return $$sizeof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State']); }");
        so.bind("$sizeof", fxn);
        fxn = Global.eval("function() { return $$alignof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State']); }");
        so.bind("$alignof", fxn);
        fxn = Global.eval("function(fld) { return $$offsetof(xdc.om['ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State'], fld); }");
        so.bind("$offsetof", fxn);
    }

    void TimerSupport$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/dm6446/TimerSupport.xs");
        om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Module", "ti.sysbios.family.c64p.dm6446");
        po.init("ti.sysbios.family.c64p.dm6446.TimerSupport.Module", om.findStrict("ti.sysbios.interfaces.ITimerSupport.Module", "ti.sysbios.family.c64p.dm6446"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("timer", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", "ti.sysbios.family.c64p.dm6446"), false), Global.newArray(new Object[]{}), "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.family.c64p.dm6446.TimerSupport$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
        // struct TimerSupport.Emulation
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport$$Emulation", "ti.sysbios.family.c64p.dm6446");
        po.init("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("suspSrc", (Proto)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.dm6446"), $$UNDEF, "w");
        // struct TimerSupport.Module_State
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport$$Module_State", "ti.sysbios.family.c64p.dm6446");
        po.init("ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", null);
                po.addFld("$hostonly", $$T_Num, 0, "r");
                po.addFld("suspSrc", new Proto.Arr((Proto)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.dm6446"), false), $$DEFAULT, "w");
    }

    void TimerSupport$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport", "ti.sysbios.family.c64p.dm6446");
        vo.bind("Emulation$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport$$Emulation", "ti.sysbios.family.c64p.dm6446");
        vo.bind("Module_State$fetchDesc", Global.newObject("type", "ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", "isScalar", false));
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport$$Module_State", "ti.sysbios.family.c64p.dm6446");
        po.bind("suspSrc$fetchDesc", Global.newObject("type", "xdc.rov.support.ScalarStructs.S_SuspSrc", "isScalar", true));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.family.c64p.dm6446.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.family.c64p.dm6446"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/family/c64p/dm6446/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.family.c64p.dm6446"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.family.c64p.dm6446"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.family.c64p.dm6446"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.family.c64p.dm6446"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.family.c64p.dm6446"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.family.c64p.dm6446"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.family.c64p.dm6446", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.family.c64p.dm6446");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.family.c64p.dm6446.");
        pkgV.bind("$vers", Global.newArray("2, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios.interfaces", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.family.c64p.dm6446'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.a64P',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.a64Pe',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.ae64P',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.ae64Pe',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.a64P', {target: 'ti.targets.C64P', suffix: '64P'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.a64Pe', {target: 'ti.targets.C64P_big_endian', suffix: '64Pe'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.ae64P', {target: 'ti.targets.elf.C64P', suffix: 'e64P'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.family.c64p.dm6446.ae64Pe', {target: 'ti.targets.elf.C64P_big_endian', suffix: 'e64Pe'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void TimerSupport$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport", "ti.sysbios.family.c64p.dm6446");
        po = (Proto.Obj)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Module", "ti.sysbios.family.c64p.dm6446");
        vo.init2(po, "ti.sysbios.family.c64p.dm6446.TimerSupport", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport$$capsule", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("$package", om.findStrict("ti.sysbios.family.c64p.dm6446", "ti.sysbios.family.c64p.dm6446"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Module__startupDoneFxn");
        icfgs.add("Module__startupDoneFxn");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("SuspSrc", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("Emulation", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", "ti.sysbios.family.c64p.dm6446"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Emulation", "ti.sysbios.family.c64p.dm6446"));
        mcfgs.add("timer");
        vo.bind("Module_State", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", "ti.sysbios.family.c64p.dm6446"));
        tdefs.add(om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.Module_State", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("SuspSrc_ARM", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_ARM", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("SuspSrc_DSP", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.SuspSrc_DSP", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("ti.sysbios.interfaces");
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.family.c64p.dm6446")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("enable", om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport.enable", "ti.sysbios.family.c64p.dm6446"));
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_family_c64p_dm6446_TimerSupport_Module__startupDone__E", "ti_sysbios_family_c64p_dm6446_TimerSupport_enable__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("TimerSupport", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("TimerSupport");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.family.c64p.dm6446.TimerSupport", "ti.sysbios.family.c64p.dm6446"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.family.c64p.dm6446.TimerSupport")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.family.c64p.dm6446")).add(pkgV);
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
        TimerSupport$$OBJECTS();
        TimerSupport$$CONSTS();
        TimerSupport$$CREATES();
        TimerSupport$$FUNCTIONS();
        TimerSupport$$SIZES();
        TimerSupport$$TYPES();
        if (isROV) {
            TimerSupport$$ROV();
        }//isROV
        $$SINGLETONS();
        TimerSupport$$SINGLETONS();
        $$INITIALIZATION();
    }
}
