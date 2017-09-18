/* 
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
/*
 *  ======== Syn.xs ========
 */

var Gbl = xdc.loadCapsule('Gbl.xs');
var Pag = xdc.loadCapsule('Pag.xs');

function PL( s ) { Gbl.outstr().println(s); }

var _curId = 1;

/*
 *  ======== genLines ========
 */
function genLines(sNode, tab, node, elemFlg, sName)
{
    if (sNode.length() == 0) return;

    var pre = Gbl.indent(tab);
    var sumText = null;
    var sumLine = null;
    var eSkip = false;
    var defAnc = null;
    var defAlt = null;

    var metaonly = Gbl.unitNode().@metaonly == '1' ? true : false;
    if (node.name() == 'decl') {
        defAnc = String(node.@anchor);
        defAlt = String(node.@altext);
        sumText = "//&nbsp;" + node.@summary;
        sumLine = "<div class='xdocSynSum'>" + pre + Gbl.decode(sumText) + "</div>";
        if (!metaonly) {
            metaonly = !Gbl.hasTargSyn(node);
    }
    }
    if (node.name() == 'group') {
        if (!elemFlg) {
            defAnc = String(node.@name).replace(/ /g, '_');
            defAlt = String(node.@name);
            if (!metaonly) {
                metaonly = !Gbl.hasTargSyn(node);
            }
        }
    }

    sumLine = null;

    for each (var ln in sNode.line) {
        if (ln.@elem.length()) {
            eSkip = ln.@elem == '1';
            continue;
        }
        if (!elemFlg && eSkip) {
            continue;
        }

        if (ln.@blank == '1' && (elemFlg || ln.@always == '1')) {
            Gbl.genRowB();
            continue;
        }

        if (!Gbl.opts().nodFlg && ln.@nodoc.length()) {
            continue;
        }

        var anc = defAnc;
        var alt = defAlt;

        if (ln.@anchor.length()) {
            anc = String(ln.@anchor);
            alt = String(ln.@altext);
        }
        if (anc && sName == Gbl.META_SYN && !metaonly) {
            anc = 'meta' + anc;
        }
        if (!elemFlg && ln.@comment.length()) {
            sumText = "//&nbsp;" + ln.@comment;
        }
        if (ln.@content.length()) {
            var cs = elemFlg ? _remGo(Gbl.mkCode(ln)) : _repGo(Gbl.mkCode(ln), anc, alt, sumText);
            Gbl.genRowC(pre + cs);
            sumText = null;
        }
        if (elemFlg && ln.@comment.length()) {
            PL("<div class='xdocSynSum'>" + pre + Gbl.indent(ln.@tab) + "//&nbsp;" + Gbl.decode(ln.@comment) + "</div>");
            sumLine = null;
        }

        if (sumLine) {
            PL(sumLine);
            sumLine = null;
        }
    }

    if (node.name() == 'group' || node.name() == 'decl' && node.@name == 'create' && sNode.name() == 'synopTarg') {
        return;
    }

    if (sumLine) {
        PL(sumLine);
    }

// FIX Gbl.sprintf !!!!
//    Gbl.TRACE = true;
//    PL(Gbl.sprintf("<div class='xdocSynSum'>$1//&nbsp;$2$3</div>", pre, Gbl.decode(node.@summary), link));
//    Gbl.TRACE = false;

}

/*
 *  ======== genPrefix ========
 */
function genPrefix(node, sName)
{
    var left, right;
    var cls;
    var id;
    var isDecl = node.name() == 'decl' || node.name() == 'group';
    var isUnit = node.name() == 'unit';

    if (sName == Gbl.META_SYN) {
        if (isDecl) {
            cls = 'xdocSynMdcl';
            id = 'xdoc-id-meta2dcl';
        }
        else {
            cls = 'xdocSynM';
            id = 'xdoc-id-meta2';
        }
        left = Gbl.sprintf("<tt>$1</tt>Configuration settings", Gbl.span(id, ''));
        right = '';
        id = 'meta-synop';
    }
    else if (sName == Gbl.TARG_SYN) {
        if (isDecl) {
            cls = 'xdocSynTdcl';
            id = 'xdoc-id-targ2dcl';
        }
        else {
            cls = 'xdocSynT';
            id = 'xdoc-id-targ2';
        }
        left = Gbl.sprintf("<tt>$1</tt> synopsis", Gbl.span(id, 'C'));
        right = 'target-domain';
        id = 'targ-synop';
    }
    else {
        cls = isDecl ? 'xdocSynXdcl' : 'xdocSynX';
        id = 'spec-synop';
    }

    PL(Gbl.sprintf("<span id='$1'></span>", id));

    if (sName == Gbl.SPEC_SYN) {
            Pag.genSpecFile(Gbl.unitNode(), 'declarations',
                            isDecl ? 'xdoc-iddcl' : null);
    }
    else {
        if (isUnit) {
            Pag.genUnitSpecFile(node, left, right);
        }
        else {
            Pag.genLabel(left, right);
        }
    }
    Pag.genSynTabBeg(cls);
}

/*
 *  ======== genSuffix ========
 */
function genSuffix()
{
    Pag.genSynTabEnd();
}

/*
 *  ======== getDeclKind ========
 */
function getDeclKind(content)
{
    var kind = null;

    if (content.match(/>typedef</)) {
        kind = 'typedef';
    }
    else if (content.match(/>#define</)) {
        kind = 'const';
    }
    else if (content.match(/\(/)) {
        kind = 'fxn';
    }
    else if (content.match(/>struct</)) {
        kind = 'struct';
    }
    else if (content.match(/>enum</)) {
        kind = 'enum';
    }
    return (kind);
}

/*
 *  ======== getDeclName ========
 */
function getDeclName(content)
{
    var name = null;

    var mArr = content.match(/<go><span class="xdoc-id">(.*?)<\/span>.*/);
    if (mArr) {
        name = mArr[1];
    }
    return (name);
}

/*
 *  ======== getDeclTableHtml ========
 */
function getDeclTableHtml(content, anc, alt, sumText)
{
    var pre = Gbl.indent(0);
    var decl;
    var rep;

    if (sumText) {
        sumText = sumText.replace(/%3Ca/g, '%3Cspan').replace(/%3C%2Fa%3E/g, '%3C%2Fspan%3E');
        rep ='<a class="xdocSynGo" href="#' + anc + '">$1<span class="xdocSynHoverShadow"><span class="xdocSynHover">' + sumText + '</span></span></a>';
        rep = Gbl.decode(rep);
    }
    else {
        rep = '<a class="xdocSynGo" href="#' + anc + '" title="' + alt + '">$1</a>'; 
    }

    decl = Gbl.decode(pre + content);
    var idx = decl.indexOf('<go><span class="xdoc-id">');
    pre = decl.substr(0, idx);
    var suf = decl.substr(idx);
    if (pre.match(/\(\*/)) {
        pre = pre.replace(/\(\*/, '');
        suf = '(*' + suf;
    }
    decl = '<td class="xdocSynDecl" nowrap><div class="xdocSynTblType">' + pre +
           '</div></td><td class="xdocSynDecl"><div class="xdocSynTblDecl">' +
           suf + '</div></td>';
    decl = '<tr valign="top">' + decl.replace(/<go>(.*?)<\/go>/, rep) + '</tr>';
    decl = decl.replace(/<go>/g, '').replace(/<\/go>/g, '');
    return (decl);
}

/*
 *  ======== _mkLink ========
 */
function _mkLink( anc, alt )
{
    return Gbl.sprintf("<a class='xdocSynGo' href='$1$2.html#$3' title='$4.$2.$5'>&nbsp;</a>",
        Gbl.path(),
        String(Gbl.unitNode().@name),
        anc,
        String(Gbl.pkgNode().@name),
        alt
    );
}

/*
 *  ======== _remGo ========
 */
function _remGo( s )
{
    return(s.replace(/<go>/g, '').replace(/<\/go>/g, ''));
}

/*
 *  ======== _repGo ========
 */
function _repGo( s, anc, alt, sum )
{
    var rep;
    var popup = '';

    if (sum) {
        sum = sum.replace(/%3Ca/g, '%3Cspan').replace(/%3C%2Fa%3E/g, '%3C%2Fspan%3E');
        rep ='<a class="xdocSynGo" href="#' + anc + '">$1<span class="xdocSynHoverShadow"><span class="xdocSynHover">' + sum + '</span></span></a>';
    }
    else {
        rep = '<a class="xdocSynGo" href="#' + anc + '" title="' + alt + '">$1</a>'; 
    }

    return _remGo(s.replace(/<go>(.*?)<\/go>/, rep));
}

/*
 *  @(#) xdc.tools.cdoc; 1, 0, 0, 0,338; 12-19-2013 19:54:19; /db/ztree/library/trees/xdc/xdc-z63x/src/packages/
 */

