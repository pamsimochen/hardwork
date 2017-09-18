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

public class google_targets
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("google.targets.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("google.targets", new Value.Obj("google.targets", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("google.targets.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("google.targets.ITarget", new Value.Obj("google.targets.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("google.targets.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "google.targets"));
        om.bind("google.targets.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets"));
        om.bind("google.targets.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "google.targets"));
        om.bind("google.targets.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets"));
        om.bind("google.targets.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets"));
        om.bind("google.targets.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets"));
        om.bind("google.targets.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets"));
        om.bind("google.targets.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets"));
        om.bind("google.targets.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets"));
        om.bind("google.targets.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets"));
        om.bind("google.targets.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets"));
        om.bind("google.targets.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "google.targets"));
        om.bind("google.targets.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "google.targets"));
    }

    void ITarget$$CONSTS()
    {
        // interface ITarget
    }

    void ITarget$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITarget$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITarget$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "google/targets/ITarget.xs");
        om.bind("google.targets.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("google.targets.ITarget.Module", "google.targets");
        po.init("google.targets.ITarget.Module", om.findStrict("xdc.bld.ITarget3.Module", "google.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("stdInclude", $$T_Str, "google/targets/std.h", "rh");
        po.addFld("dllExt", $$T_Str, ".so", "wh");
        po.addFld("GCCVERS", $$T_Str, null, "wh");
        po.addFld("BINVERS", $$T_Str, null, "wh");
        po.addFld("version", $$T_Str, $$UNDEF, "wh");
        po.addFld("GCCTARG", $$T_Str, null, "wh");
        po.addFld("LONGNAME", $$T_Str, "/bin/gcc", "wh");
        po.addFld("libcDir", $$T_Str, $$UNDEF, "wh");
        po.addFld("fsDir", $$T_Str, $$UNDEF, "wh");
        po.addFld("CYGWIN", $$T_Bool, false, "rh");
        po.addFld("noStdLinkScript", $$T_Bool, false, "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-g", "defs", "-D_DEBUG_=1"), "linkOpts", "-g")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2 -ffunction-sections -fdata-sections"), "linkOpts", "-Wl,--gc-sections")})}), "wh");
        po.addFld("versionMap", new Proto.Map($$T_Str), Global.newArray(new Object[]{Global.newArray(new Object[]{"gcc3.2", "1,0,3.2,0"})}), "wh");
        po.addFld("remoteHost", $$T_Str, $$UNDEF, "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets"), Global.newObject("cmd", "$(rootDir)/$(GCCTARG)/bin/ar", "opts", "cr"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets"), Global.newObject("cmd", "$(rootDir)/$(LONGNAME)", "opts", ""), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets"), Global.newObject("cmd", "$(rootDir)/$(LONGNAME) -c -MD -MF $@.dep", "opts", ""), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "google.targets"), Global.newObject("cmd", "$(rootDir)/$(LONGNAME) -c -x assembler", "opts", ""), "rh");
        po.addFld("includeOpts", $$T_Str, "", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("google.targets.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("google.targets.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("google.targets.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "google.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "google.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "google.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "google.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "google.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "google.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "google.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "google.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "google.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "google.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "google.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "google.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "google.targets"), fxn);
    }

    void ITarget$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("google.targets.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "google.targets"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "google/targets/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "google.targets"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "google.targets"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "google.targets"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "google.targets"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "google.targets"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "google.targets"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "google.targets", Value.DEFAULT, false);
        pkgV.bind("$name", "google.targets");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "google.targets.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.bld", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['google.targets'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ITarget$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("google.targets.ITarget", "google.targets");
        po = (Proto.Obj)om.findStrict("google.targets.ITarget.Module", "google.targets");
        vo.init2(po, "google.targets.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("google.targets.ITarget$$capsule", "google.targets"));
        vo.bind("$package", om.findStrict("google.targets", "google.targets"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "google.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "google.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "google.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "google.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "google.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "google.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "google.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "google.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "google.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "google.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "google.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "google.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "google.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "google.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "google.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITarget", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITarget");
        vo.seal(null);
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("google.targets.ITarget")).bless();
        ((Value.Arr)om.findStrict("$packages", "google.targets")).add(pkgV);
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
        ITarget$$OBJECTS();
        ITarget$$CONSTS();
        ITarget$$CREATES();
        ITarget$$FUNCTIONS();
        ITarget$$SIZES();
        ITarget$$TYPES();
        if (isROV) {
            ITarget$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        $$INITIALIZATION();
    }
}
