<h2>Basic TIR-FCS correlation functions</h2>
<h3>FCCS framework</h3>
$$insert:framework_schwille.inc$$
<h3>The TIR MDF and correlation function</h3>
We assume the MDF of a TIRF$$ref:SANKARAN2010:J. Sankaran, X. Shi, L. Ho, E. Stelzer, and T. Wohland, "ImFCS: A software for Imaging FCS data analysis and visualization," Opt. Express  18, 25468-25481 (2010).$$ to be
  $$bmath:\mbox{MDF}_\text{g/r}(x,y)= \frac{1}{\mathcal{N}}\cdot\left[f_\text{pixel,g/r}(x,y)\ast \mbox{PSF}_\text{g/r}(x,y,z)\right]= \frac{1}{\mathcal{N}}\cdot I_\text{LS}(z)\cdot\int_{0}^a\int_{0}^b\mbox{PSF}_\text{g/r}(x-\mu,y-\nu,z)\;\dd\nu\;\dd\mu,$$
where $$math:\ast$$ is a convolution and $$math:f_\text{pixel,g/r}(x,y)$$ describes the pixel extent in x- and y-direction:
  $$bmath:f_\text{pixel,g/r}(x,y)=\begin{cases}1&0\leq x\leq a\ \cap\ 0\leq y\leq b\\0&\text{else}\end{cases}$$
Then the non-normalized crosscorrelation function for species X and between channels g and r can be written as:
  $$bmath:G_\text{gr}^{X}(\tau)=\eta_\text{g}\eta_\text{r}c_\text{X}\cdot G_\text{gr,x}^{X}(\tau)\cdot G_\text{gr,y}^{X}(\tau)$$
where for TIR-FC(C)S the x- and y-components are equal:
  $$bmath:G_\text{gr,x}^{X}(\tau)= \int_{-\infty}^\infty\int_{-\infty}^\infty\mbox{MDF}_\text{g}(x)\cdot\mbox{MDF}_\text{r}(x')\cdot \frac{1}{\sqrt{4\pi D_X\tau}}\cdot\exp\left[-\frac{(x-x')^2}{4D_X\tau}\right]\;\mathrm{d}x\;\mathrm{d}x'\ =\ \frac{1}{2a^2}\cdot\left\{ \left[(a-d_x)\cdot\erf\left(\frac{\sqrt{2}\cdot(a-d_x)}{\sqrt{8D_X\tau+w_g^2+w_r^2}}\right)+2d_x\cdot\erf\left(\frac{\sqrt{2}\cdot d_x}{\sqrt{8 D_X\tau+w_g^2+w_r^2}}\right)+(a+d_x)\cdot\erf\left(\frac{\sqrt{2}\cdot(a+d_x)}{\sqrt{8D_X\tau+w_g^2+w_r^2}}\right)\right]+\frac{\sqrt{8D_X\tau+w_g^2+w_r^2}}{\sqrt{2\pi}}\cdot\left[e^{-\frac{2\cdot(a-d_x)^2}{8D_X\tau+w_g^2+w_r^2}}-2\cdot e^{-\frac{2d_x^2}{8D_X\tau+w_g^2+w_r^2}}+e^{-\frac{2\cdot(a+d_x)^2}{8D_X\tau+w_g^2+w_r^2}}\right]\right\}    $$


For details, see $$ref:KRIEGER2013:JW.Krieger, AP Singh, etal. (2013):"SPIM-FCCS", to be published$$