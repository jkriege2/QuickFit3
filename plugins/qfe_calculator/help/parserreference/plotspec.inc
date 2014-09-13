The plotspec string specifies, how the plot should be set up. It consists of one specifier out of every column below (in the given order). You can also omit columns, if you want to use the default value. The errorbar column does only apply to errorbar plots and the symbol and line columns might be ignored for some plots (e.g. <tt>bar()</tt>).

<center><table border="1" padding="2" spacing="1">
  <tr style="background: silver"><td><b>color</b></td><td><b>symbol</b></td><td><b>line</b></td><td><b>errorbar (optional)</b></td></tr>
  <tr><td><tt>r</tt>: red</td><td><tt>o</tt>/<tt>of</tt>: (filled) circle</td><td><tt>-</tt>: solid line</td><td><tt>EB</tt>: error bars</td></tr>
  <tr><td><tt>g</tt>: green</td><td><tt>x</tt>: cross</td><td><tt>--</tt>: dashed line</td><td><tt>EL</tt>: error lines</td></tr>
  <tr><td><tt>b</tt>: blue</td><td><tt>+</tt>: plus</td><td><tt>:</tt>: dotted line</td><td><tt>EP</tt>: error polygons</td></tr>
  <tr><td><tt>c</tt>: cyan</td><td><tt>*</tt> / <tt>*f</tt>: (filled) star</td><td><tt>-.</tt>: dash-dot line</td><td><tt>ES</tt>: simple error bars </td></tr>
  <tr><td><tt>m</tt>: magenta</td><td><tt>^</tt> / <tt>^f</tt>: (filled) triangle</td><td><tt>-..</tt>: dash-dot-dot line</td><td><tt>EE</tt> error ellipses</td></tr>
  <tr><td><tt>y</tt>: yellow</td><td><tt>v</tt> / <tt>vf</tt>: (filled) inverted triangle</td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td><tt>k</tt>: black</td><td><tt>'</tt>: dot</td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td></td><td><tt>d</tt> / <tt>df</tt> (filled) diamond</td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td></td><td><tt>s</tt> / <tt>sf</tt>: (filled) square</td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td></td><td></td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td><tt>rgb(<i>r</i>,<i>g</i>,<i>b</i>)</tt>: the color specified by <i>r,g,b</i></td><td><tt>SS(<i>d</i>)</tt>: symbol size <i>d</i> in points </td><td><tt>lw(<i>w</i>)</tt>: linewidth <i>w</i> in points </td><td><tt></tt> </td></tr>
  <tr><td><tt>rgba(<i>r</i>,<i>g</i>,<i>b</i>,<i>a</i>)</tt>: the color specified by <i>r,g,b,a</i></td><td></td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td><tt>gray(<i>g</i>)</tt>: given gray-level</td><td></td><td><tt></tt> </td><td><tt></tt> </td></tr>
  <tr><td><tt>gray(<i>g</i>,<i>a</i>)</tt>: given gray-level and alpha</td><td></td><td><tt></tt> </td><td><tt></tt> </td></tr>
</table></center>
$$note:The color values <i>r,g,b,a</i> range from 0 to 255.$$