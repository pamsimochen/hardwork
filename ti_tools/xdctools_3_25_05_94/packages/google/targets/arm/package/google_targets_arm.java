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

public class google_targets_arm
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
        Global.callFxn("loadPackage", xdcO, "xdc.bld");
        Global.callFxn("loadPackage", xdcO, "gnu.targets");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "google.targets");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("google.targets.arm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("google.targets.arm", new Value.Obj("google.targets.arm", pkgP));
    }

    void Bionic$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("google.targets.arm.Bionic.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("google.targets.arm.Bionic", new Value.Obj("google.targets.arm.Bionic", po));
        pkgV.bind("Bionic", vo);
        // decls 
        om.bind("google.targets.arm.Bionic.Model", om.findStrict("xdc.bld.ITarget.Model", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.Extension", om.findStrict("xdc.bld.ITarget.Extension", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.Command", om.findStrict("xdc.bld.ITarget2.Command", "google.targets.arm"));
        om.bind("google.targets.arm.Bionic.Options", om.findStrict("xdc.bld.ITarget2.Options", "google.targets.arm"));
    }

    void Bionic$$CONSTS()
    {
        // module Bionic
    }

    void Bionic$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Bionic$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Bionic$$SIZES()
    {
    }

    void Bionic$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "google/targets/arm/Bionic.xs");
        om.bind("google.targets.arm.Bionic$$capsule", cap);
        po = (Proto.Obj)om.findStrict("google.targets.arm.Bionic.Module", "google.targets.arm");
        po.init("google.targets.arm.Bionic.Module", om.findStrict("google.targets.ITarget.Module", "google.targets.arm"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Bionic", "rh");
        po.addFld("os", $$T_Str, "Linux", "rh");
        po.addFld("suffix", $$T_Str, "bnc", "rh");
        po.addFld("isa", $$T_Str, "v5T", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "google.targets.arm"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "google.targets.arm.rts", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.arm", "wh");
        po.addFld("LONGNAME", $$T_Str, $$UNDEF, "wh");
        po.addFld("libcDir", $$T_Str, $$UNDEF, "wh");
        po.addFld("fsDir", $$T_Str, $$UNDEF, "wh");
        po.addFld("stdInclude", $$T_Str, "google/targets/arm/std.h", "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets.arm"), Global.newObject("cmd", "$(rootDir)/$(LONGNAME) -c -MD -MF $@.dep", "opts", "-march=armv5t -fno-short-enums "), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "google.targets.arm"), Global.newObject("prefix", "-fPIC -Wunused", "suffix", "-Dfar=  -D_ANDROID_"), "wh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets.arm"), Global.newObject("cmd", "$(rootDir)/$(LONGNAME) -c -x assembler", "opts", "-march=armv5t"), "rh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "google.targets.arm"), Global.newObject("prefix", "", "suffix", "-Wl,-Map=$(XDCCFGDIR)/$@.map "), "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets.arm"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 4L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 4L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 4L), "t_LLong", Global.newObject("size", 8L, "align", 4L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("google.targets.arm.Bionic$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("google.targets.arm.Bionic$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("google.targets.arm.Bionic$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "google.targets.arm"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "google.targets.arm"), fxn);
    }

    void Bionic$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("google.targets.arm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "google.targets.arm"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "google.targets.arm", Value.DEFAULT, false);
        pkgV.bind("$name", "google.targets.arm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "google.targets.arm.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        imports.add(Global.newArray("gnu.targets", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['google.targets.arm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Bionic$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("google.targets.arm.Bionic", "google.targets.arm");
        po = (Proto.Obj)om.findStrict("google.targets.arm.Bionic.Module", "google.targets.arm");
        vo.init2(po, "google.targets.arm.Bionic", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("google.targets.arm.Bionic$$capsule", "google.targets.arm"));
        vo.bind("$package", om.findStrict("google.targets.arm", "google.targets.arm"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "google.targets.arm"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets.arm"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "google.targets.arm"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets.arm"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets.arm"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets.arm"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets.arm"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets.arm"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets.arm"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "google.targets.arm"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets.arm"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets.arm"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "google.targets.arm"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "google.targets.arm"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "google.targets.arm"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("google.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "google.targets.arm")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("google.targets.arm.Bionic$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Bionic", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Bionic");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("google.targets.arm.Bionic", "google.targets.arm"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("google.targets.arm.Bionic")).bless();
        ((Value.Arr)om.findStrict("$packages", "google.targets.arm")).add(pkgV);
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
        Bionic$$OBJECTS();
        Bionic$$CONSTS();
        Bionic$$CREATES();
        Bionic$$FUNCTIONS();
        Bionic$$SIZES();
        Bionic$$TYPES();
        if (isROV) {
            Bionic$$ROV();
        }//isROV
        $$SINGLETONS();
        Bionic$$SINGLETONS();
        $$INITIALIZATION();
    }
}
