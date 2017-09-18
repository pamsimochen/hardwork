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

public class ti_sysbios_rta
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
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "ti.sysbios.knl");
        Global.callFxn("loadPackage", xdcO, "xdc.rov");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.sysbios.rta.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.sysbios.rta", new Value.Obj("ti.sysbios.rta", pkgP));
    }

    void Agent$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rta.Agent.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rta.Agent", new Value.Obj("ti.sysbios.rta.Agent", po));
        pkgV.bind("Agent", vo);
        // decls 
        om.bind("ti.sysbios.rta.Agent.Transport", new Proto.Enm("ti.sysbios.rta.Agent.Transport"));
    }

    void SupportLogger$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.sysbios.rta.SupportLogger.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.sysbios.rta.SupportLogger", new Value.Obj("ti.sysbios.rta.SupportLogger", po));
        pkgV.bind("SupportLogger", vo);
        // decls 
    }

    void Agent$$CONSTS()
    {
        // module Agent
        om.bind("ti.sysbios.rta.Agent.Transport_RTDX", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rta.Agent.Transport", "ti.sysbios.rta"), "ti.sysbios.rta.Agent.Transport_RTDX", 0));
        om.bind("ti.sysbios.rta.Agent.Transport_STOP_MODE_ONLY", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rta.Agent.Transport", "ti.sysbios.rta"), "ti.sysbios.rta.Agent.Transport_STOP_MODE_ONLY", 1));
        om.bind("ti.sysbios.rta.Agent.Transport_USER", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.sysbios.rta.Agent.Transport", "ti.sysbios.rta"), "ti.sysbios.rta.Agent.Transport_USER", 2));
    }

    void SupportLogger$$CONSTS()
    {
        // module SupportLogger
    }

    void Agent$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void SupportLogger$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Agent$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void SupportLogger$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Agent$$SIZES()
    {
    }

    void SupportLogger$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Agent$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rta/Agent.xs");
        om.bind("ti.sysbios.rta.Agent$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rta.Agent.Module", "ti.sysbios.rta");
        po.init("ti.sysbios.rta.Agent.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("loadLogging", $$T_Bool, true, "wh");
        po.addFld("loadLoggingRuntimeControl", $$T_Bool, true, "wh");
        po.addFld("loadLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 512L, "wh");
        po.addFld("loadLoggerBufferSection", $$T_Str, "[Default]", "wh");
        po.addFld("mainLogging", $$T_Bool, true, "wh");
        po.addFld("mainLoggingRuntimeControl", $$T_Bool, true, "wh");
        po.addFld("mainLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
        po.addFld("mainLoggerBufferSection", $$T_Str, "[Default]", "wh");
        po.addFld("sysbiosHwiLogging", $$T_Bool, false, "wh");
        po.addFld("sysbiosHwiLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("sysbiosSwiLogging", $$T_Bool, false, "wh");
        po.addFld("sysbiosSwiLoggingRuntimeControl", $$T_Bool, false, "wh");
        po.addFld("sysbiosTaskLogging", $$T_Bool, true, "wh");
        po.addFld("sysbiosTaskLoggingRuntimeControl", $$T_Bool, true, "wh");
        po.addFld("sysbiosLoggerSize", Proto.Elm.newCNum("(xdc_SizeT)"), 4096L, "wh");
        po.addFld("sysbiosLoggerBufferSection", $$T_Str, "[Default]", "wh");
        po.addFld("configureSystemLog", $$T_Bool, true, "wh");
        po.addFld("numSystemRecords", Proto.Elm.newCNum("(xdc_Int)"), 64L, "wh");
        po.addFld("configureLoadLog", $$T_Bool, true, "wh");
        po.addFld("numLoadRecords", Proto.Elm.newCNum("(xdc_Int)"), 64L, "wh");
        po.addFld("periodInMs", Proto.Elm.newCNum("(xdc_Int)"), 100L, "wh");
        po.addFld("priority", Proto.Elm.newCNum("(xdc_Int)"), 1L, "wh");
        po.addFld("agentStackSize", Proto.Elm.newCNum("(xdc_SizeT)"), 1024L, "wh");
        po.addFld("agentStackSection", $$T_Str, "[Default]", "wh");
        po.addFld("transferBufferSize", Proto.Elm.newCNum("(xdc_Int)"), 85L, "wh");
        po.addFld("transferBufferSection", $$T_Str, $$UNDEF, "wh");
        po.addFld("addSupportLogger", $$T_Bool, true, "wh");
        po.addFld("supportControl", $$T_Bool, true, "wh");
        po.addFld("transport", (Proto)om.findStrict("ti.sysbios.rta.Agent.Transport", "ti.sysbios.rta"), om.find("ti.sysbios.rta.Agent.Transport_STOP_MODE_ONLY"), "wh");
        po.addFld("dataStream", $$T_Str, "/rtdx0", "wh");
        po.addFld("commandStream", $$T_Str, "/rtdx1", "wh");
        po.addFld("responseStream", $$T_Str, "/rtdx1", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rta.Agent$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rta.Agent$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rta.Agent$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void SupportLogger$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rta/SupportLogger.xs");
        om.bind("ti.sysbios.rta.SupportLogger$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.sysbios.rta.SupportLogger.Module", "ti.sysbios.rta");
        po.init("ti.sysbios.rta.SupportLogger.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.sysbios.rta"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
            po.addFld("LM_timestampFreq", (Proto)om.findStrict("xdc.runtime.Log$$Event", "ti.sysbios.rta"), Global.newObject("mask", Global.eval("0x0100 | 0x0200"), "msg", "LM_timestampFreq: Timestamp Frequency hi: %d, lo: %d"), "w");
            po.addFld("createLogSize", Proto.Elm.newCNum("(xdc_Int)"), 32L, "w");
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.sysbios.rta.SupportLogger$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.sysbios.rta.SupportLogger$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.sysbios.rta.SupportLogger$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.sysbios.rta.SupportLogger$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Agent$$ROV()
    {
    }

    void SupportLogger$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rta.SupportLogger", "ti.sysbios.rta");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.sysbios.rta.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.sysbios.rta"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/sysbios/rta/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.sysbios.rta"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.sysbios.rta"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.sysbios.rta"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.sysbios.rta"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.sysbios.rta"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.sysbios.rta"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.sysbios.rta", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.sysbios.rta");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.sysbios.rta.");
        pkgV.bind("$vers", Global.newArray("2, 0, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.sysbios.rta'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.a28L',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.a28FP',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.a64P',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.a64Pe',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.a674',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae430X',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae430XS',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ar430XS',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae64P',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae64Pe',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae674',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae64T',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae66',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae66e',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aearp32',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aearp32F',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.ae9',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aea8f',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aea8fnv',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aem3',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aem4',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aem4f',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.am3g',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.am4g',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.am4fg',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aa8fg',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aa9fg',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.aa15fg',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.arm3',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.arm4',\n");
            sb.append("'lib/sysbios/debug/ti.sysbios.rta.arm4f',\n");
            sb.append("'lib/smpbios/debug/ti.sysbios.rta.aem3',\n");
            sb.append("'lib/smpbios/debug/ti.sysbios.rta.aem4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.a28L', {target: 'ti.targets.C28_large', suffix: '28L'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.a28FP', {target: 'ti.targets.C28_float', suffix: '28FP'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.a64P', {target: 'ti.targets.C64P', suffix: '64P'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.a64Pe', {target: 'ti.targets.C64P_big_endian', suffix: '64Pe'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.a674', {target: 'ti.targets.C674', suffix: '674'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae430X', {target: 'ti.targets.msp430.elf.MSP430X', suffix: 'e430X'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae430XS', {target: 'ti.targets.msp430.elf.MSP430X_small', suffix: 'e430XS'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae64P', {target: 'ti.targets.elf.C64P', suffix: 'e64P'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae64Pe', {target: 'ti.targets.elf.C64P_big_endian', suffix: 'e64Pe'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae674', {target: 'ti.targets.elf.C674', suffix: 'e674'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae64T', {target: 'ti.targets.elf.C64T', suffix: 'e64T'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae66', {target: 'ti.targets.elf.C66', suffix: 'e66'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae66e', {target: 'ti.targets.elf.C66_big_endian', suffix: 'e66e'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aearp32', {target: 'ti.targets.arp32.elf.ARP32', suffix: 'earp32'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aearp32F', {target: 'ti.targets.arp32.elf.ARP32_far', suffix: 'earp32F'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.ae9', {target: 'ti.targets.arm.elf.Arm9', suffix: 'e9'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aea8f', {target: 'ti.targets.arm.elf.A8F', suffix: 'ea8f'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aea8fnv', {target: 'ti.targets.arm.elf.A8Fnv', suffix: 'ea8fnv'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aem3', {target: 'ti.targets.arm.elf.M3', suffix: 'em3'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.am3g', {target: 'gnu.targets.arm.M3', suffix: 'm3g'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aa8fg', {target: 'gnu.targets.arm.A8F', suffix: 'a8fg'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aa9fg', {target: 'gnu.targets.arm.A9F', suffix: 'a9fg'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.aa15fg', {target: 'gnu.targets.arm.A15F', suffix: 'a15fg'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.arm3', {target: 'iar.targets.arm.M3', suffix: 'rm3'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
            sb.append("['lib/sysbios/debug/ti.sysbios.rta.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
            sb.append("['lib/smpbios/debug/ti.sysbios.rta.aem3', {target: 'ti.targets.arm.elf.M3', suffix: 'em3'}],\n");
            sb.append("['lib/smpbios/debug/ti.sysbios.rta.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Agent$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rta.Agent", "ti.sysbios.rta");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rta.Agent.Module", "ti.sysbios.rta");
        vo.init2(po, "ti.sysbios.rta.Agent", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rta.Agent$$capsule", "ti.sysbios.rta"));
        vo.bind("$package", om.findStrict("ti.sysbios.rta", "ti.sysbios.rta"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Transport", om.findStrict("ti.sysbios.rta.Agent.Transport", "ti.sysbios.rta"));
        vo.bind("Transport_RTDX", om.findStrict("ti.sysbios.rta.Agent.Transport_RTDX", "ti.sysbios.rta"));
        vo.bind("Transport_STOP_MODE_ONLY", om.findStrict("ti.sysbios.rta.Agent.Transport_STOP_MODE_ONLY", "ti.sysbios.rta"));
        vo.bind("Transport_USER", om.findStrict("ti.sysbios.rta.Agent.Transport_USER", "ti.sysbios.rta"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rta")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.sysbios.rta.Agent$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Agent", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Agent");
    }

    void SupportLogger$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.sysbios.rta.SupportLogger", "ti.sysbios.rta");
        po = (Proto.Obj)om.findStrict("ti.sysbios.rta.SupportLogger.Module", "ti.sysbios.rta");
        vo.init2(po, "ti.sysbios.rta.SupportLogger", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.sysbios.rta.SupportLogger$$capsule", "ti.sysbios.rta"));
        vo.bind("$package", om.findStrict("ti.sysbios.rta", "ti.sysbios.rta"));
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
        mcfgs.add("LM_timestampFreq");
        mcfgs.add("createLogSize");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.sysbios.rta")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_sysbios_rta_SupportLogger_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray("LM_timestampFreq"));
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.seal("length");
        vo.bind("MODULE_STARTUP$", 1);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("SupportLogger", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("SupportLogger");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rta.Agent", "ti.sysbios.rta"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.sysbios.rta.SupportLogger", "ti.sysbios.rta"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.sysbios.rta.Agent")).bless();
        ((Value.Obj)om.getv("ti.sysbios.rta.SupportLogger")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.sysbios.rta")).add(pkgV);
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
        Agent$$OBJECTS();
        SupportLogger$$OBJECTS();
        Agent$$CONSTS();
        SupportLogger$$CONSTS();
        Agent$$CREATES();
        SupportLogger$$CREATES();
        Agent$$FUNCTIONS();
        SupportLogger$$FUNCTIONS();
        Agent$$SIZES();
        SupportLogger$$SIZES();
        Agent$$TYPES();
        SupportLogger$$TYPES();
        if (isROV) {
            Agent$$ROV();
            SupportLogger$$ROV();
        }//isROV
        $$SINGLETONS();
        Agent$$SINGLETONS();
        SupportLogger$$SINGLETONS();
        $$INITIALIZATION();
    }
}
