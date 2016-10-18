<?xml version="1.0" encoding="ISO-8859-1"?> 
<xsl:stylesheet version="2.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
<xsl:template match="/"> 
<html> 
<body> 
  <h2>ChangeLog for Emergent Version <xsl:value-of select="$versNum"/>
  Revisions <xsl:value-of select="$revRange"/> </h2> 
<table border="1" style="table-layout:fixed;width: 100%;">
    <tr>
        <td width="5%">Rev No.</td>
        <td width="10%">
                Date               
        </td>
        <td width="5%">
                    Author
                 
        </td>
        <td width="">
                    Message
                 
        </td>
    </tr>
    <xsl:for-each select="log/logentry"> 
        <tr>         
            <td>             
                <pre><xsl:value-of select="@revision"/> </pre>
            </td>        
            <td>
                <pre><xsl:value-of select="substring(date, 0, 11)" /></pre>
            </td>    
            <td>
                <pre>
                    <xsl:value-of select="author"/>
                </pre>
            </td>    
            <td>         
                <pre>
                    <xsl:value-of select="msg"/>
                </pre>
            </td>
        </tr>
    </xsl:for-each> 
</table> 
</body> 
</html> 
</xsl:template> 
</xsl:stylesheet> 
