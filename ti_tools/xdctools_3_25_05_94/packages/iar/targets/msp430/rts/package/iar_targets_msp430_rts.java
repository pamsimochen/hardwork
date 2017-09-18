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

public class iar_targets_msp430_rts
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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("iar.targets.msp430.rts.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("iar.targets.msp430.rts", new Value.Obj("iar.targets.msp430.rts", pkgP));
    }

    void $$SINGLETONS()
    {
        pkgP.init("iar.targets.msp430.rts.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "iar.targets.msp430.rts"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "iar/targets/msp430/rts/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "iar.targets.msp430.rts"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "iar.targets.msp430.rts"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "iar.targets.msp430.rts"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "iar.targets.msp430.rts"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "iar.targets.msp430.rts"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "iar.targets.msp430.rts"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "iar.targets.msp430.rts", Value.DEFAULT, false);
        pkgV.bind("$name", "iar.targets.msp430.rts");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "iar.targets.msp430.rts.");
        pkgV.bind("$vers", Global.newArray("1, 0, 0"));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['iar.targets.msp430.rts'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/debug/iar.targets.msp430.rts.ar430',\n");
            sb.append("'lib/release/iar.targets.msp430.rts.ar430',\n");
            sb.append("'lib/debug_full/iar.targets.msp430.rts.ar430',\n");
            sb.append("'lib/release_full/iar.targets.msp430.rts.ar430',\n");
            sb.append("'lib/debug/iar.targets.msp430.rts.ar430XL',\n");
            sb.append("'lib/release/iar.targets.msp430.rts.ar430XL',\n");
            sb.append("'lib/debug_full/iar.targets.msp430.rts.ar430XL',\n");
            sb.append("'lib/release_full/iar.targets.msp430.rts.ar430XL',\n");
            sb.append("'lib/debug/iar.targets.msp430.rts.ar430XS',\n");
            sb.append("'lib/release/iar.targets.msp430.rts.ar430XS',\n");
            sb.append("'lib/debug_full/iar.targets.msp430.rts.ar430XS',\n");
            sb.append("'lib/release_full/iar.targets.msp430.rts.ar430XS',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/debug/iar.targets.msp430.rts.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/release/iar.targets.msp430.rts.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/debug_full/iar.targets.msp430.rts.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/release_full/iar.targets.msp430.rts.ar430', {target: 'iar.targets.msp430.MSP430', suffix: 'r430'}],\n");
            sb.append("['lib/debug/iar.targets.msp430.rts.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/release/iar.targets.msp430.rts.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/debug_full/iar.targets.msp430.rts.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/release_full/iar.targets.msp430.rts.ar430XL', {target: 'iar.targets.msp430.MSP430X_large', suffix: 'r430XL'}],\n");
            sb.append("['lib/debug/iar.targets.msp430.rts.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
            sb.append("['lib/release/iar.targets.msp430.rts.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
            sb.append("['lib/debug_full/iar.targets.msp430.rts.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
            sb.append("['lib/release_full/iar.targets.msp430.rts.ar430XS', {target: 'iar.targets.msp430.MSP430X_small', suffix: 'r430XS'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Arr)om.findStrict("$packages", "iar.targets.msp430.rts")).add(pkgV);
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
