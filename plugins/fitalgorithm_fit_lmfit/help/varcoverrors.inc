<h2>Parameter error estimates</h2>
<p>This fit algorithms can calculate an estimate of the fit parameter errors by evaluating the variance-covariance matrix $(\mat{C})$ of the fit problem at the ideal solution $(\vec{p})$. The errors are then:
$[ \mbox{err}_i=\frac{\chi^2}{N-p}\cdot\sqrt{\mat{C}_{p,p}} ]$
where $(N)$ is the nuber of datapoints, $(p)$ is the number of parameters and:
$[\chi^2(\vec{p})=\sum_{i=1}^N\left(\frac{y_i-f(x_i,\vec{p})}{\sigma_i}\right)^2. ]$
The variance-covariance matrix is estimated from the numerical Jacobian:
$[ \mat{C}=\left(\mat{J}^T \mat{J}\right)^{-1} ]$
with the Jacobi matrix
$[ \mat{J}_{i,j}=\frac{1}{\sigma_i}\cdot\left.\frac{\partial f(x,\vec{p})}{\partial p_j}\right|_{x=x_i} ]$

</p>