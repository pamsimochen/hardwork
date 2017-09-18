/* 
 *  Copyright (c) 2011 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * 
 */
/*!
 *  ======== google.targets.ITarget ========
 *  Interface to Google's GCC compatible compilers (e.g. Bionic)
 */
@TargetHeader("xdc/bld/stddefs.xdt")
metaonly interface ITarget inherits xdc.bld.ITarget3 {

    override readonly config string stdInclude = "google/targets/std.h";
    override config string dllExt     = ".so";

    /*!
     *  ======== GCCVERS ========
     *  Version number of the GCC compiler; e.g., "3.2".
     *
     *  This string can be supplied by the user, otherwise it is obtained
     *  by running "gcc -dumpversion".
     */
    config string GCCVERS = null;

    /*!
     *  ======== BINVERS ========
     *  Version number of binutils used with the compiler; e.g., "2.19".
     *
     *  This string can be supplied by the user, otherwise it is obtained
     *  by running "ld -v".
     */
    config string BINVERS = null;

    /*!
     *  ======== version ========
     *  The Compatibility Key associated with this target.
     *
     *  The first two components of this target's Compatibility Key are '1,0'.
     *  The rest of the Key represents the compiler version. The third
     *  component combines the major and the minor version number in the format
     *  Major.Minor. The fourth component is the patch number.
     *
     *  @a(Example)
     *  If this target's `rootDir` points to the compiler version 3.4.6, the
     *  Compatibility Key is [1,0,3.4,6].
     *
     */
    override metaonly config String version;

    /*!
     *  ======== GCCTARG ========
     *  The name of the platform executing programs produced by this target
     *
     *  This string can be supplied by the user, otherwise is is obtained
     *  from the compiler and follows the GNU standard format
     *  (<cpu>-<manufacturer>-<os> or <cpu>-<manufacturer>-<kernel>-<os>);
     *  e.g., "sparc-sun-solaris2.6" or "i586-pc-linux-gnu".
     *
     *  When building a GCC compiler, there are three different execution
     *  platforms to consider: the platform used to "build" the compiler, the
     *  "host" platform that runs the compiler, and the "target" platform
     *  that runs the executables produced by the compiler. All three
     *  platforms are identified using a
     *  {@link http://sources.redhat.com/autobook/autobook/autobook_17.html configuration name}
     *  defined by GNU Autotools.  `GCCTARG` is the name of the "target"
     *  platform.
     */
    config string GCCTARG = null;

    /*!
     *  ======== LONGNAME ========
     *  The "long name" of the gcc compiler
     *
     *  This name is used (in conjunction with rootDir) to find the compiler
     *  and linker for this target. The format of `LONGNAME` is always
     *  "/bin/<machine>-gcc". For majority of the targets, the default value
     *  for `LONGNAME` does not ever need to be changed. But, there are
     *  targets where the different but compatible compilers may have
     *  different `LONGNAME` parameters. For such targets and compilers,
     *  `LONGNAME` can be set in `config.bld`.
     *
     *  @a(Example)
     *  If a version 2010q1 of the CodeSourcery GNU toolchain for Arm is
     *  installed in C:/CodeSourcery/arm-2010q1, the following settings in
     *  `config.bld` configure `google.targets.arm.Bionic` target to use that
     *  toolchain:
     *  @p(code)
     *  var Bionic = xdc.module("google.targets.arm.Bionic");
     *  Bionic.rootDir = "C:/CodeSourcery/arm-2010q1";
     *  Bionic.LONGNAME = "bin/arm-none-linux-gnueabi-gcc";
     *  Bionic.libcDir = "C:/android-rowboat/bionic";
     *  Bionic.fsDir = "C:/android-rowboat/out/target/product/ti814xevm/obj";
     *  @p
     *
     */
    config string LONGNAME = "/bin/gcc";

    /*!
     *  ======== libcDir ========
     *  The libc of the gcc compiler
     *
     *  This name is used to find the libc libraries.
     *
     *  @a(Example)
     *  If the libc files are installed in C:/android-rowboat/bionic, the
     *  following settings in `config.bld` configure the
     *  `google.targets.arm.Binoic` target to use that libc installation:
     *  @p(code)
     *  var Bionic = xdc.module("google.targets.arm.Bionic");
     *  Bionic.rootDir = "C:/CodeSourcery/arm-2010q1";
     *  Bionic.LONGNAME = "bin/arm-none-linux-gnueabi-gcc";
     *  Bionic.libcDir = "C:/android-rowboat/bionic";
     *  Bionic.fsDir = "C:/android-rowboat/out/target/product/ti814xevm/obj";
     *  @p
     *
     */
    config string libcDir;

    /*!
     *  ======== fsDir ========
     *  The bionic file system
     *
     *  This name is used to find the file system libraries.
     *
     *  @a(Example)
     *  If the file system is installed in
     *  C:/android-rowboat/out/target/product/ti814xevm/obj, the
     *  following settings in `config.bld` configure the
     *  `google.targets.arm.Binoic` target to use that file system:
     *  @p(code)
     *  var Bionic = xdc.module("google.targets.arm.Bionic");
     *  Bionic.rootDir = "C:/CodeSourcery/arm-2010q1";
     *  Bionic.LONGNAME = "bin/arm-none-linux-gnueabi-gcc";
     *  Bionic.fsDir = "C:/android-rowboat/out/target/product/ti814xevm/obj";
     *  @p
     *
     */
    config string fsDir;

    /*!
     *  ======== CYGWIN ========
     *  Is the target's compiler a cygwin executable
     *
     *  Since file names produced by cygwin-based tools differ from the
     *  names understood by other Windows executables, it is important
     *  to avoid using the names output by cygwin tools as input to
     *  non-cygwin programs.  This property tells the target whether
     *  or not it's possible to use the output from `gcc -MD -MF`, for
     *  example.
     */
    readonly config Bool CYGWIN = false;

    /*!
     *  ======== noStdLinkScript ========
     *  Don't use the standard linker script
     *
     *  If `true`, add a `-T` flag before the generated `package/cfg/*.xdl`
     *  file passed to the linker.  This flag suppresses use of the
     *  standard linker script implicit in the GCC flow, which effectively
     *  says the generated `.xdl` file assumes total control for all
     *  `MEMORY` and `SECTION` directives.
     *
     */
    config Bool noStdLinkScript = false;

    /*
     *  ======== profiles ========
     */
    override config xdc.bld.ITarget.OptionSet profiles[string] = [
        ["debug", {
            compileOpts: {
                copts: "-g",
                defs:  "-D_DEBUG_=1",
            },
            linkOpts: "-g",
        }],

        ["release", {
            compileOpts: {
                copts: "-O2 -ffunction-sections -fdata-sections",
            },
            linkOpts: "-Wl,--gc-sections",
        }]
    ];

    /*!
     *  ======== versionMap ========
     *  Map of GCC compiler version numbers to compatibility keys.
     *
     *  This map translates version string information from the compiler
     *  into a compatibility key.  The compatibilty key is used to
     *  validate consistency among a collection of packages used in
     *  a configuration.
     *
     *  The compiler version string is "gcc<ver>", where <ver> is
     *  GCCVERS.
     *
     *  If a compiler version is not found in this map the default is
     *  "1,0,<ver>", where <ver> is the compiler version number.  Thus,
     *  the user only needs to extend this table when a significant
     *  incompatibility occurs or when two versions of the compiler should
     *  be treated as 100% compatible.
     */
    override config string versionMap[string] = [
        ["gcc3.2", "1,0,3.2,0"],
    ];

    /*!
     *  ======== remoteHost ========
     *  Remote host used to run compiler, linker, and archiver tools
     *
     *  If `remoteHost` is `null` (or `undefined`), the configured compiler
     *  is run locally; otherwise, `remoteHost` is taken to be the host name
     *  of the machine that that should be used to run the specified compiler.
     *
     *  All target commands are prefixed with a command that uses `rsh` to run
     *  the commands on the specified host.  Thus, in order to use this
     *  setting, the remote machine must be support `rsh` and the user must
     *  have permission to run commands from the local machine on the remote
     *  host named `remoteHost`.  This usually involves adding a line to the
     *  user's `~/.rhosts` file on the remote machine of the form:
     *  @p(code)
     *      local-machine-name user-name
     *  @p
     *  where `local-machine-name` is the name of the local machine and
     * `user-name` is the user's login name on the local machine.
     */
    config string remoteHost;

    /*!
     *  ======== ar ========
     *  The command used to create an archive
     */
    override readonly config xdc.bld.ITarget2.Command ar = {
        cmd: "$(rootDir)/$(GCCTARG)/bin/ar",
        opts: "cr"
    };

    /*!
     *  ======== lnk ========
     *  The command used to link executables.
     */
    override readonly config xdc.bld.ITarget2.Command lnk = {
        cmd: "$(rootDir)/$(LONGNAME)",
        opts: ""
    };

    /*!
     *  ======== cc ========
     *  The command used to compile C/C++ source files into object files
     */
    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "$(rootDir)/$(LONGNAME) -c -MD -MF $@.dep",
        opts: ""
    };

    /*!
     *  ======== asm ========
     *  The command used to assembles assembly source files into object files
     */
    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "$(rootDir)/$(LONGNAME) -c -x assembler",
        opts: ""
    };

    /*!
     *  ======== includeOpts ========
     *  Additional user configurable target-specific include path options
     */
    override config string includeOpts = "";
}
/*
 *  @(#) google.targets; 1, 0, 0,139; 12-19-2013 15:12:08; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

