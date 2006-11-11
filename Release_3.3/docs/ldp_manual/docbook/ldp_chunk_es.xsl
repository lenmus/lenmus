<?xml version='1.0'?>

<!--
    English version
-->


<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:import href="C:/USR/CosasMias/Software/DocBook/docbook-xsl-1.70.1/html/chunk.xsl"/>
<!--<xsl:param name="html.stylesheet" select="'singledoc.css'"/>-->
<xsl:param name="admon.graphics" select="1"/>
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="section.label.includes.component.label" select="1"/>
<!--<xsl:param name="navig.graphics" select="1"/>-->
<xsl:param name="generate.legalnotice.link" select="1"/>
<xsl:param name="chunker.output.doctype-public" select="'-//W3C//DTD HTML 4.01//EN'"/>
<xsl:param name="chunker.output.doctype-system" select="'http://www.w3.org/TR/html4/strict.dtd'"/>


<!-- change the word Home with Table of Contents for English output -->
<xsl:param name="local.l10n.xml" select="document('')" />
<l:i18n xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0">
 <l:l10n language="en">
  <l:gentext key="nav-home" text="Table of contents"/>
 </l:l10n>
</l:i18n>

<!-- Remove body attributes -->
<xsl:template name="body.attributes">
</xsl:template>



<!-- ==================================================================== -->

<xsl:template name="chunk-element-content">
  <xsl:param name="prev"/>
  <xsl:param name="next"/>
  <xsl:param name="nav.context"/>
  <xsl:param name="content">
    <xsl:apply-imports/>
  </xsl:param>

  <xsl:call-template name="user.preroot"/>

  <html>
    <!--<xsl:call-template name="html.head">
      <xsl:with-param name="prev" select="$prev"/>
      <xsl:with-param name="next" select="$next"/>
    </xsl:call-template>-->

<!-- ================================================================================ -->
<head>
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1" />
<title>LenMus - Free software for music theory</title>
<meta http-equiv="expires" content="0" />
<meta name="resource-type" content="document" />
<meta name="distribution" content="global" />
<meta name="author" content="Cecilio Salmeron" />
<meta name="copyright" content="Copyright (c) 2003-2005 Cecilio Salmeron" />
<meta name="keywords" lang="en" content="music theory, music language,
    side reading, music, intervals, scales, chords, music exercises, ear training" />
<meta name="keywords" lang="es" content="teoria de la musica, lenguaje musical, musica,
    intervalos, escalas, acordes, solfeo, lectura de partituras, programas gratuitos de musica,
    software gratis de musica" />
<meta name="description" lang="en" content="Free music software. Free software for learning
    and practising music theory and language. Software for ear training." />
<meta name="description" lang="es" content="Software gratuito para aprendizaje de la
     teoria y el lenguaje musical. Ejercicios de intervalos, solfeo, acordes y escalas.
     Educacion musical del oido." />
<meta name="robots" content="index, follow" />
<meta name="revisit-after" content="30 days" />
<meta name="rating" content="general" />
<link rel="shortcut icon" href="./layout/images/favicon.ico" type="image/x-icon" />
<script type="text/JavaScript" src="./layout/scripts/scripts.js"></script>
<link rel="top" title="Home" href="http://www.lenmus.org/" />
<link rel="icon" href="./layout/images/favicon.gif" type="image/png" />
<link rel="stylesheet" media="all" href="./layout/style/screen3.css" type="text/css" />
</head>

<body>
<div id="wrap">

        <!-- header BEGIN -->
        <div id="lm-header">
            <!--<div id="lm-header-logo"></div>-->

            <div id="lm-header-items">
              <ul>
                <li><a title="Ver esta web en Espanol" href="../es/index.html">Espanol</a></li>
                <li><a title="View this site in English" href="#">English</a></li>
              </ul>
            </div>
        </div>

        <!-- header navigation bar -->
        <!-- <div id="lm-nav"></div> -->
        <!-- header END -->

        <!-- leftside BEGIN -->
        <div id="lm-side-left">
            <!-- Busqueda Google -->
            <div class="lm-search-box">
                <object>       <!-- this is required for valid XML -->
                    <form method="get" action="http://www.google.com/search">
                        <a href="http://www.google.com/">
                        <img src="./layout/images/LogoGoogle.gif" border="0" alt="Google"/></a>
                        <input type="text" name="q" maxlength="64" value="" class="searchTextCtrl" />
                        <button type="submit" name="btnG" value="Search this site" class="searchBtn">Search this site</button>
                        <input type="hidden" name="domains" value="www.lenmus.org" class="searchBtn" />
                        <input type="hidden" name="sitesearch" value="www.lenmus.org" class="searchBtn" />
                    </form>
                </object>
            </div>

            <!-- main navigation menu on the left -->
            <div id="lm-left-menu">
                <ul id="lm-side-nav">
                  <li><a title="Portada" href="page.php?pid=noticias&amp;lang=Spa">Portada</a></li>
                  <li><span class="lm-side-nav">General</span>
                  <ul>
                    <li><a title="LenMus en SourceForge" href="http://sourceforge.net/projects/lenmus" target="_blank&amp;lang=Spa">LenMus en SourceForge</a></li>
                    <li><a title="Boletin de noticias" href="page.php?pid=newsletter&amp;lang=Spa">Boletin de noticias</a></li>
                    <li><a title="Contactar" href="page.php?pid=paginas&amp;name=contact&amp;lang=Spa">Contactar</a></li>
                    <li><a title="Politica de privacidad" href="page.php?pid=paginas&amp;name=privacy&amp;lang=Spa">Politica de privacidad</a></li>
                    <li><a title="Licencia" href="page.php?pid=paginas&amp;name=license&amp;lang=Spa">Licencia</a></li>
                  </ul></li>

                  <li><span class="lm-side-nav">El programa LenMus</span>
                  <ul>
                    <li><a title="Descargas" href="page.php?pid=paginas&amp;name=downloads&amp;lang=Spa">Descargas</a></li>
                    <li><a title="Objetivos" href="page.php?pid=paginas&amp;name=roadmap&amp;lang=Spa">Objetivos</a></li>
                    <li><a title="Imagenes de pantallas" href="page.php?pid=paginas&amp;name=screenshots&amp;lang=Spa">Imagenes de pantallas</a></li>
                    <li><a title="Funcionalidades" href="page.php?pid=paginas&amp;name=features&amp;lang=Spa">Funcionalidades</a></li>
                    <li><a title="Novedades" href="page.php?pid=paginas&amp;name=new_in_version&amp;lang=Spa">Novedades</a></li>
                    <li><a title="Instalacion" href="page.php?pid=paginas&amp;name=installation&amp;lang=Spa">Instalacion</a></li>
                  </ul></li>

                  <li><span class="lm-side-nav">Ayuda</span>
                  <ul>
                    <li><a title="Documentacion" href="page.php?pid=paginas&amp;name=docs_index&amp;lang=Spa">Documentacion</a></li>
                    <li><a title="Comunicar un error" href="page.php?pid=paginas&amp;name=send_bugs&amp;lang=Spa">Comunicar un error</a></li>
                  </ul></li>

                  <li><a title="Involucrate" href="page.php?pid=paginas&amp;name=get_involved&amp;lang=Spa">Involucrate</a>
                  <ul>
                    <li><a title="Programadores" href="page.php?pid=paginas&amp;name=join_developers&amp;lang=Spa">Programadores</a></li>
                  </ul></li>

                  <li><span class="lm-side-nav">Developers</span>
                  <ul>
                    <li><a title="Log of changes" href="page.php?pid=paginas&amp;name=changelog&amp;lang=Spa">Log of changes</a></li>
                    <li><a title="wxMidi" href="page.php?pid=paginas&amp;name=wxmidi&amp;lang=Spa">wxMidi</a></li>
                  </ul></li>
                </ul>
            </div>

            <!-- left side blocks -->
            <div id="lm-left-side-block">
                <a id="NoPatents" href="http://swpat.ffii.org" alt="No software patents logo"></a>
            </div>

            <div id="lm-left-side-block">
                <a href="http://sourceforge.net">
                  <img src="./layout/images/sourceforge_logo.png"
                    width="125" height="37" border="0" alt="SourceForge.net Logo" /></a>
            </div>

            <div id="lm-left-side-block">
                <a id="wxWidgets" href="http://www.wxwidgets.org" alt="wxWidgets logo"></a>
            </div>

        </div>

        <!-- leftside END -->

        <!-- content BEGIN -->
        <div id="lm-content-wrapper">
            <div id="lm-content">

<!-- Start of content code -->
    
     <!-- <xsl:call-template name="body.attributes"/>
      <xsl:call-template name="user.header.navigation"/>-->

      <xsl:call-template name="header.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

      <xsl:call-template name="user.header.content"/>

      <xsl:copy-of select="$content"/>

      <xsl:call-template name="user.footer.content"/>

      <xsl:call-template name="footer.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

      <!--<xsl:call-template name="user.footer.navigation"/>-->
      
<!-- End of content code -->

            </div>      <!-- lm-content -->
        </div>      <!-- lm-content-wrapper -->

        <!-- footer BEGIN -->
        <div id="lm-footer">
            Copyright (c) 2003-2006 Cecilio Salmeron. All rights reserved.
            <br /><a rel="license" href="http://www.lenmus.org/sw/page.php?pid=paginas&amp;name=license">
            License</a>
        </div>
        <!-- footer END -->


</div>  <!-- wrap -->

    </body>
  </html>
</xsl:template>

<!-- ==================================================================== -->
</xsl:stylesheet>
