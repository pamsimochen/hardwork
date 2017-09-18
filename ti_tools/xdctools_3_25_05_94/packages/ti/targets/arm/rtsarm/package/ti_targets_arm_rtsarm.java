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

public class ti_targets_arm_rtsarm
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
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.targets.arm.rtsarm.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.targets.arm.rtsarm", new Value.Obj("ti.targets.arm.rtsarm", pkgP));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.targets.arm.rtsarm.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.targets.arm.rtsarm"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/targets/arm/rtsarm/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.targets.arm.rtsarm"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.targets.arm.rtsarm"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.targets.arm.rtsarm"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.targets.arm.rtsarm"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.targets.arm.rtsarm"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.targets.arm.rtsarm"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.targets.arm.rtsarm", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.targets.arm.rtsarm");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.targets.arm.rtsarm.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("xdc.runtime", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.targets.arm.rtsarm'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/ti.targets.arm.rtsarm.ae7',\n");
            sb.append("'lib/boot.ae7',\n");
            sb.append("'lib/auto_init.ae7',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.ae7e',\n");
            sb.append("'lib/boot.ae7e',\n");
            sb.append("'lib/auto_init.ae7e',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.ae9',\n");
            sb.append("'lib/boot.ae9',\n");
            sb.append("'lib/auto_init.ae9',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.ae9t',\n");
            sb.append("'lib/boot.ae9t',\n");
            sb.append("'lib/auto_init.ae9t',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aea8f',\n");
            sb.append("'lib/boot.aea8f',\n");
            sb.append("'lib/auto_init.aea8f',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aea8ft',\n");
            sb.append("'lib/boot.aea8ft',\n");
            sb.append("'lib/auto_init.aea8ft',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aea8fv',\n");
            sb.append("'lib/boot.aea8fv',\n");
            sb.append("'lib/auto_init.aea8fv',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aea8fnv',\n");
            sb.append("'lib/boot.aea8fnv',\n");
            sb.append("'lib/auto_init.aea8fnv',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aea15fnv',\n");
            sb.append("'lib/boot.aea15fnv',\n");
            sb.append("'lib/auto_init.aea15fnv',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aem3',\n");
            sb.append("'lib/boot.aem3',\n");
            sb.append("'lib/auto_init.aem3',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aem3e',\n");
            sb.append("'lib/boot.aem3e',\n");
            sb.append("'lib/auto_init.aem3e',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aem4',\n");
            sb.append("'lib/boot.aem4',\n");
            sb.append("'lib/auto_init.aem4',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aem4f',\n");
            sb.append("'lib/boot.aem4f',\n");
            sb.append("'lib/auto_init.aem4f',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4',\n");
            sb.append("'lib/boot.aer4',\n");
            sb.append("'lib/auto_init.aer4',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4e',\n");
            sb.append("'lib/boot.aer4e',\n");
            sb.append("'lib/auto_init.aer4e',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4t',\n");
            sb.append("'lib/boot.aer4t',\n");
            sb.append("'lib/auto_init.aer4t',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4te',\n");
            sb.append("'lib/boot.aer4te',\n");
            sb.append("'lib/auto_init.aer4te',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4ft',\n");
            sb.append("'lib/boot.aer4ft',\n");
            sb.append("'lib/auto_init.aer4ft',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4fe',\n");
            sb.append("'lib/boot.aer4fe',\n");
            sb.append("'lib/auto_init.aer4fe',\n");
            sb.append("'lib/ti.targets.arm.rtsarm.aer4fte',\n");
            sb.append("'lib/boot.aer4fte',\n");
            sb.append("'lib/auto_init.aer4fte',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/ti.targets.arm.rtsarm.ae7', {target: 'ti.targets.arm.elf.Arm7', suffix: 'e7'}],\n");
            sb.append("['lib/boot.ae7', {target: 'ti.targets.arm.elf.Arm7', suffix: 'e7'}],\n");
            sb.append("['lib/auto_init.ae7', {target: 'ti.targets.arm.elf.Arm7', suffix: 'e7'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.ae7e', {target: 'ti.targets.arm.elf.Arm7_big_endian', suffix: 'e7e'}],\n");
            sb.append("['lib/boot.ae7e', {target: 'ti.targets.arm.elf.Arm7_big_endian', suffix: 'e7e'}],\n");
            sb.append("['lib/auto_init.ae7e', {target: 'ti.targets.arm.elf.Arm7_big_endian', suffix: 'e7e'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.ae9', {target: 'ti.targets.arm.elf.Arm9', suffix: 'e9'}],\n");
            sb.append("['lib/boot.ae9', {target: 'ti.targets.arm.elf.Arm9', suffix: 'e9'}],\n");
            sb.append("['lib/auto_init.ae9', {target: 'ti.targets.arm.elf.Arm9', suffix: 'e9'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.ae9t', {target: 'ti.targets.arm.elf.Arm9t', suffix: 'e9t'}],\n");
            sb.append("['lib/boot.ae9t', {target: 'ti.targets.arm.elf.Arm9t', suffix: 'e9t'}],\n");
            sb.append("['lib/auto_init.ae9t', {target: 'ti.targets.arm.elf.Arm9t', suffix: 'e9t'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aea8f', {target: 'ti.targets.arm.elf.A8F', suffix: 'ea8f'}],\n");
            sb.append("['lib/boot.aea8f', {target: 'ti.targets.arm.elf.A8F', suffix: 'ea8f'}],\n");
            sb.append("['lib/auto_init.aea8f', {target: 'ti.targets.arm.elf.A8F', suffix: 'ea8f'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aea8ft', {target: 'ti.targets.arm.elf.A8Ft', suffix: 'ea8ft'}],\n");
            sb.append("['lib/boot.aea8ft', {target: 'ti.targets.arm.elf.A8Ft', suffix: 'ea8ft'}],\n");
            sb.append("['lib/auto_init.aea8ft', {target: 'ti.targets.arm.elf.A8Ft', suffix: 'ea8ft'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aea8fv', {target: 'ti.targets.arm.elf.A8Fv', suffix: 'ea8fv'}],\n");
            sb.append("['lib/boot.aea8fv', {target: 'ti.targets.arm.elf.A8Fv', suffix: 'ea8fv'}],\n");
            sb.append("['lib/auto_init.aea8fv', {target: 'ti.targets.arm.elf.A8Fv', suffix: 'ea8fv'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aea8fnv', {target: 'ti.targets.arm.elf.A8Fnv', suffix: 'ea8fnv'}],\n");
            sb.append("['lib/boot.aea8fnv', {target: 'ti.targets.arm.elf.A8Fnv', suffix: 'ea8fnv'}],\n");
            sb.append("['lib/auto_init.aea8fnv', {target: 'ti.targets.arm.elf.A8Fnv', suffix: 'ea8fnv'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aea15fnv', {target: 'ti.targets.arm.elf.A15Fnv', suffix: 'ea15fnv'}],\n");
            sb.append("['lib/boot.aea15fnv', {target: 'ti.targets.arm.elf.A15Fnv', suffix: 'ea15fnv'}],\n");
            sb.append("['lib/auto_init.aea15fnv', {target: 'ti.targets.arm.elf.A15Fnv', suffix: 'ea15fnv'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aem3', {target: 'ti.targets.arm.elf.M3', suffix: 'em3'}],\n");
            sb.append("['lib/boot.aem3', {target: 'ti.targets.arm.elf.M3', suffix: 'em3'}],\n");
            sb.append("['lib/auto_init.aem3', {target: 'ti.targets.arm.elf.M3', suffix: 'em3'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aem3e', {target: 'ti.targets.arm.elf.M3_big_endian', suffix: 'em3e'}],\n");
            sb.append("['lib/boot.aem3e', {target: 'ti.targets.arm.elf.M3_big_endian', suffix: 'em3e'}],\n");
            sb.append("['lib/auto_init.aem3e', {target: 'ti.targets.arm.elf.M3_big_endian', suffix: 'em3e'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/boot.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/auto_init.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/boot.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/auto_init.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4', {target: 'ti.targets.arm.elf.R4', suffix: 'er4'}],\n");
            sb.append("['lib/boot.aer4', {target: 'ti.targets.arm.elf.R4', suffix: 'er4'}],\n");
            sb.append("['lib/auto_init.aer4', {target: 'ti.targets.arm.elf.R4', suffix: 'er4'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4e', {target: 'ti.targets.arm.elf.R4_big_endian', suffix: 'er4e'}],\n");
            sb.append("['lib/boot.aer4e', {target: 'ti.targets.arm.elf.R4_big_endian', suffix: 'er4e'}],\n");
            sb.append("['lib/auto_init.aer4e', {target: 'ti.targets.arm.elf.R4_big_endian', suffix: 'er4e'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4t', {target: 'ti.targets.arm.elf.R4t', suffix: 'er4t'}],\n");
            sb.append("['lib/boot.aer4t', {target: 'ti.targets.arm.elf.R4t', suffix: 'er4t'}],\n");
            sb.append("['lib/auto_init.aer4t', {target: 'ti.targets.arm.elf.R4t', suffix: 'er4t'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4te', {target: 'ti.targets.arm.elf.R4t_big_endian', suffix: 'er4te'}],\n");
            sb.append("['lib/boot.aer4te', {target: 'ti.targets.arm.elf.R4t_big_endian', suffix: 'er4te'}],\n");
            sb.append("['lib/auto_init.aer4te', {target: 'ti.targets.arm.elf.R4t_big_endian', suffix: 'er4te'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4ft', {target: 'ti.targets.arm.elf.R4Ft', suffix: 'er4ft'}],\n");
            sb.append("['lib/boot.aer4ft', {target: 'ti.targets.arm.elf.R4Ft', suffix: 'er4ft'}],\n");
            sb.append("['lib/auto_init.aer4ft', {target: 'ti.targets.arm.elf.R4Ft', suffix: 'er4ft'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4fe', {target: 'ti.targets.arm.elf.R4F_big_endian', suffix: 'er4fe'}],\n");
            sb.append("['lib/boot.aer4fe', {target: 'ti.targets.arm.elf.R4F_big_endian', suffix: 'er4fe'}],\n");
            sb.append("['lib/auto_init.aer4fe', {target: 'ti.targets.arm.elf.R4F_big_endian', suffix: 'er4fe'}],\n");
            sb.append("['lib/ti.targets.arm.rtsarm.aer4fte', {target: 'ti.targets.arm.elf.R4Ft_big_endian', suffix: 'er4fte'}],\n");
            sb.append("['lib/boot.aer4fte', {target: 'ti.targets.arm.elf.R4Ft_big_endian', suffix: 'er4fte'}],\n");
            sb.append("['lib/auto_init.aer4fte', {target: 'ti.targets.arm.elf.R4Ft_big_endian', suffix: 'er4fte'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Arr)om.findStrict("$packages", "ti.targets.arm.rtsarm")).add(pkgV);
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
        if (isROV) {
        }//isROV
        $$SINGLETONS();
        $$INITIALIZATION();
    }
}
