/* 
 *  Copyright (c) 2009 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */

/*!
 *  ======== IProduct ========
 *  Interface specifying product details. 
 *
 *  This interface allows products describe themselves for the benefit
 *  of development tools such as IDE's.  For example, a tool
 *  can read the information provided by a product that implements this
 *  interface and generate an eclipse plugin that leverages the product's
 *  "resources".
 *
 *  By expressing the product's information in this interface, a product
 *  can be delivered into multiple development tools without explicit
 *  knowledge of these tools.  For example, an online "product repository"
 *  can extract sufficient information from this interface to enable it
 *  to provide a searchable catalog of interesting products.
 *
 *  This is an experimental interface and is subject to change
 */
metaonly interface IProduct
{
    /*!
     *  ======== HelpToc ========    
     *  Help table of contents (TOC) element
     *
     *  A help TOC element defines a file, URL, or a secondary table of
     *  contents that is referenced from a global table of contents.
     *
     *  @field(label)    Summary description of help element which
     *                   is displayed in the global table of contents.
     *  @field(filePath) Relative path to help documentation file or a
     *                   secondary table of contents file or a URL. Relative
     *                   paths are relative to the directories specified in
     *                   {@link #docsLocArr}; files are searched for, in
     *                   order, within each directory and the first file
     *                   found that matches the name is used.
     *  @field(tocFile)  Flag indicates whether the element is a 
     *                   table of contents (TOC) file. Tools may
     *                   process TOC files in a special way. For
     *                   example, a tool to generate eclipse plugins
     *                   must copy the TOC file to the plugin folder
     *                   for integrating with eclipse help.
     */
    struct HelpToc {
        String label;       /*! Help element description */   
        String filePath;    /*! Path to help doc */
        Bool   tocFile;     /*! Indicates whether element is a TOC file  */
    };
     
    /*!
     *  ========= Target ==============
     *  Structure containing information on RTSC targets
     *
     *  Products may specify the mapping between device names and/or device
     *  family names and a RTSC target module.
     *  
     *  This information is leveraged by tools to derive an appropriate
     *  RTSC target when given a device name, device family, endiness, and
     *  OMF selection (COFF verses ELF).  In particular, if the device name
     *  and family name match `deviceNamePattern` and
     *  `deviceFamilyPattern`, the target name generated will be:
     *  @p(code)
     *      <packageName>[.elf].<baseTargetName>[_big_endian]
     *  @p
     *  where `<packageName>` and `<baseTargetName>` are the strings specified
     *  by the corespondingly named fields of this structure, and the strings
     *  `".elf"` and `"_big_endian"` are added as necessary.
     *
     *  Suppose, for example, a product specifies the following target mapping:
     *  @p(code)
     *  {
     *      deviceNamePattern:   "MSP430F5.*",
     *      deviceFamilyPattern: "MSP430.*",
     *      packageName:         "ti.targets.msp430",
     *      baseTargetName:      "MSP430X"
     *  }
     *  @p
     *  If the device specified is "`MSP430F5529`", the family is "`MSP430`",
     *  and the OMF is COFF, the generated target will be:
     *  @p(code)
     *      ti.targets.msp430.MSP430X
     *  @p
     *  On the other hand, if the OMF is ELF the target will be:
     *  @p(code)
     *      ti.targets.msp430.elf.MSP430X
     *  @p
     *
     *  @field(deviceNamePattern)   A regular expression for device names
     *  @field(deviceFamilyPattern) A regular expression for device families
     *  @field(packageName)         The name of a package containing a
     *                              target module that  can be used with
     *                              the  devices that match the
     *                              device name and family patterns
     *  @field(baseTargetName)      The name of the module 
     *                              (without the package prefix) in 
     *                              the package named by `packageName`
     *  @field(sourceFile)          The path to a file that defines the mapping
     *                              in a JSON file
     */
    struct Target {
        String deviceNamePattern;   /*! Device name regex eg. MSP430*  */
        String deviceFamilyPattern; /*! Device family regex eg. C6* */
        String compilerOptsPattern; /*! Compiler opts regx; e.g. .*-me .* */
        String packageName;         /*! Package containing RTSC target  */
        String baseTargetName;      /*! Name of RTSC target module */
        String sourceFile;          /*! Name of a file with target mapping */
    };

    /*!
     *  ======== UrlDescriptor ========
     *  URL Descriptor structure
     *
     *  @field(url)         Uniform Resource Locator(URL)
     *  @field(text)        Summary text for the URL
     *  @field(targetText)  Optional summary text for any target content
     *                      portion 
     */
    struct UrlDescriptor {
        String url;         /*! Uniform Resource Locator(URL) */
        String text;        /*! Summary text for the URL */
        String targetText;  /*! (optional) Summary for target portion */
    };

    /*!
     *  ======== PluginDescriptor ========
     *  Plugin Descriptor structure
     *
     *  @field(id)      Plugin id
     *  @field(version) Plugin version
     *  @field(path)    Path to plugin relative to `IProduct` implementation
     */
    struct PluginDescriptor {
        String id;          /*! Plugin id */
        String version;     /*! Plugin version */
        String path;        /*! Plugin path */
    };

    /*!
     *  ======== MacroDescriptor ========
     *  Macro Descriptor structure
     */
    struct MacroDescriptor {
        String name;     /*! Macro name */
        String desc;     /*! Macro description */
    };
    
    /*!
     *  ======== name ========
     *  Product name
     *
     *  String containing product name eg. System BIOS
     */
    config String name;

    /*!
     *  ======== id ========
     *  A unique product id 
     *
     *  For example, product developers may choose to  follow namespace
     *  conventions for Java packages or Eclipse plugins to specify a
     *  unique id like `org.eclipse.rtsc.xdctools.product`.
     */
    config String id;

    /*!
     *  ======== version ========
     *  Product version
     *
     *  The product version should follow the format 
     *  -`major.minor.service.qualifier`  where 
     *  `major`,`minor` and `service` are integers and 
     *  `qualifier` is a string. Example - 3.16.02.31-eng. 
     *
     *  Products may  specify their own guidelines for
     *  updating `major`,`minor`,`service` and `qualifier` portions of the
     *  version number as long as they are unique for the product.
     *  Products may also choose to follow popular versioning conventions
     *  like those followed for versioning Eclipse plugins.
     */
    config String version;

    /*!
     *  ======== featureId ========
     *  A unique feature id
     *
     *  @_nodoc - Deprecated
     *
     *  Feature ids are used by a system to install product upgrades.
     *  Typically a system will look at various versions of a
     *  feature available in the system and allow the user to upgrade
     *  to the latest version. The product developer can use the `featureId`
     *  to manage the granularity of product upgrades in a system. For 
     *  example the version `3.20.0.00` for product 
     *  `org.eclipse.rtsc.xdctools.product` may have the `featureId`
     *  `org.eclipse.rtsc.xdctools.product_3.20`.
     *  The product developer may allow the upgrade system to pick up
     *  patch releases to `3.20.0.00` release by specifying different
     *  versions of the product (eg. 3.20.0.01) with `featureId` 
     *  `org.eclipse.rtsc.xdctools.product_3.20`
     */
    config String featureId;

    /*!
     *  ======== updateSite ========
     *  Update site for product
     *
     *  @field(url) the `url` field should contain the URL containing
     *  product upgrades.
     *
     *  @field(text) the `text` field should contain summary description of
     *  the update site
     */
    config  UrlDescriptor updateSite;


    /*!
     *  ======== companyName ========
     *  Name of company releasing product
     *
     *  Example : "Texas Instruments"
     */
    config String companyName;

    /*!
     *  ======== productDescriptor ========
     *  Product description
     *
     *  @field(url) the `url` field should contain a product-specific "home"
     *  page URL or, if there is none, the URL of organization
     *  releasing the product. 
     *
     *  @field(text) the `text` field should contain summary description of
     *  the target content portion of the product.  
     */
    config UrlDescriptor productDescriptor;

    /*!
     *  ======== licenseDescriptor ========
     *  License information for product
     *
     *  @field(url) field should contain the URL containing the 
     *  license text.
     *
     *  @field(text) field should contain a summary description of the license
     */
    config UrlDescriptor licenseDescriptor;

    /*!
     *  ======== copyRightNotice ========
     *  Copyright notice for product
     */
    config String copyRightNotice;
    
    /*!
     *  ======== repositoryArr ========
     *  Repositories contained in the product
     *
     *  Repositories are specified relative to the product installation
     *  directory.
     */
    config String repositoryArr[];

    /*!
     *  ======== docsLocArr ========
     *  Directories containing documents in the product
     *
     *  Directories are specified relative to the product installation
     *  directory.  These directories are searched when resolving URI links
     *  to specific pages within the docs.
     */
    config String docsLocArr[];

    /*!
     *  ======== templateModule ========
     *  Name of module implementing {@link xdc.tools.product.IProductTemplate}
     *
     *  Products that deliver examples provide a module that implements the
     *  {@link xdc.tools.product.IProductTemplate} interface.  This module
     *  must be delivered in one of the repositories defined in
     *  {@link #repositoryArr}.
     */
    config String templateModule;
    
    /*!
     *  ======== productViewModule ========
     *  Name of module implementing {@link xdc.tools.product.IProductView}
     *
     *  Products must provide a module that implements the
     *  {@link xdc.tools.product.IProductView} interface.  This 
     *  module defines the "top-level" modules that are visible to the user
     *  and must be delivered in the repositories defined in
     *  {@link #repositoryArr}.
     */
    config String productViewModule;

    /*!
     *  ======== bundleName ========
     *  Product bundle name
     *  
     *  Product bundle name is embedded in the top level folder name of
     *  the product. Example: The top level folder for XDCtools version 
     *  3.16.02.31-eng is `xdctools_3_16_02_31-eng`. In this case the 
     *  bundle name is `xdctools`.
     *  The bundle name is used by tools to discover the product 
     *  installed on a filesystem.
     */
    config String bundleName;

    /*!
     *  ======== targetArr ========
     *  RTSC target modules contained in product
     *
     *  The RTSC target modules should be delivered
     *  in the repositories defined in {@link #repositoryArr}
     */
    config Target targetArr[];
    
    /*!
     *  ======== helpTocArr ========
     *  Array of table of contents elements in product
     * 
     *  Tools generating table of contents are required
     *  to preserve the order of elements specified in this array
     */ 
    config HelpToc helpTocArr[];

    /*!
     *  ======== tocIndexFile ========
     *  Top level index file referred in table of contents
     */
    config String tocIndexFile;

    /*!
     *  ======== exclusive ========
     *  This product is a singlton
     *
     *  This flag indicates whether multiple versions of this product are
     *  accessable within the IDE or whether only the latest version is
     *  available.
     *
     *  When flag is set to `true` it indicates that only one 
     *  version of the product can be active in the system. Otherwise
     *  the system can handle multiple versions of the same product. 
     */
    config Bool exclusive = false;

    /*!
     *  ======== otherFiles ========
     *  Array of paths to files that describe the product
     *
     *  A product may wish to supply files that describing certain aspects
     *  of itself. For example a product may provide text, images and video
     *  files that provide branding information for the product.
     *  These files may then be consumed by tools like IDE's via 
     *  hooks defined in the IDE framework.
     *
     *  The source file path is defined relative to the directory containing
     *  the implementation  of `IProduct`.  All files are copied to the base
     *  directory of the plugin; e.g., "../branding/about.ini" is copied to
     *  "about.ini" at the top of the plugin.
     *
     *  File names that begin with the prefix "tc:" are copied into "target
     *  content" plugin whereas file names with any other prefix (such as
     *  "ui:" or no prefix) are copied into the "IDE plugin".
     */
    config String otherFiles[];

    /*!
     *  ======== macro ========
     *  Macro associated with the product
     *
     *  Product macros are often used in IDEs for providing
     *  portable build environments. A unique macro name should
     *  be selected for the product.
     */
    config MacroDescriptor macro;

    /*!
     *  ======== externalPlugins ========
     *  External plugins delivered with the product
     */
    config PluginDescriptor externalPlugins[];

    /*!
     *  ======== externalRequirements ========
     *  External plugins required by this product
     */
    config String externalRequirements[];

    /*!
     *  ======== projectSpecPath ========
     *  Directory containing project spec files
     *
     *  This path is relative to the product install location.
     */
    config String projectSpecPath = "resources";
}
/*
 *  @(#) xdc.tools.product; 1, 0, 0,101; 12-7-2013 14:16:36; /db/ztree/library/trees/xdctools/xdctools-f41x/src/
 */

