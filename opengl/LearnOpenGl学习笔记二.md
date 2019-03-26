### LearnOpenGl学习笔记二

#### 光照

* 颜色

  定义物体的颜色为**物体从一个光源反射各个颜色分量的大小**

  ```C++
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // 光的颜色
  glm::vec3 toyColor(1.0f, 0.5f, 0.31f); // 物体的颜色
  glm::vec3 result = lightColor * toyColor; // = (1.0f, 0.5f, 0.31f); 最终展示的颜色
  ```

* 基础光照

  冯氏光照模型

  Gouraud着色 在顶点着色器中实现 高效（顶点比片段少） 不真实

  冯氏着色 在片段着色器中实现

  * 环境光照

    全局照明

    **常量环境因子**

    ```C++
    //用光的颜色乘以一个很小的常量环境因子，再乘以物体的颜色
    void main()
    {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
    
        vec3 result = ambient * objectColor;
        FragColor = vec4(result, 1.0);
    }
    ```

  * 漫反射光照

    物体上与光线方向**越接近**的片段能从光源处获得**更多的亮度**

    **法向量** + 定向光线 计算夹角

    法向量与光线向量点乘结果 **漫反射分量**

    (漫反射分量 + 环境光分量) * 光

    * 法向量

      把法线数据添加到顶点数据中

    * 法线矩阵

      计算定向光线时 使用了世界坐标系中的坐标 也应对法向量执行相应的转换

      位移不影响 执行 旋转和缩放

      不等比缩放会破坏法线

      使用法线矩阵计算

      **模型矩阵左上角的逆矩阵的转置矩阵**

      在绘制之前你最好用CPU计算出法线矩阵，然后通过**uniform**把值传递给着色器

      ```C++
      Normal = mat3(transpose(inverse(model))) * aNormal;
      //mat3失去位移属性
      ```

  * 镜面光照

    反射向量 视线向量

    **镜面强度(Specular Intensity)变量**

    视线方向与反射方向的点乘（并确保它不是负值），然后取它的32次幂(**反光度**)

    镜面光强度

* 材质

  ```
  #version 330 core
  struct Material {
      vec3 ambient; 在环境光照下这个物体反射得是什么颜色，通常这是和物体颜色相同的颜色
      vec3 diffuse; 定义了在漫反射光照下物体的颜色。（和环境光照一样）
      vec3 specular; 镜面光照对物体的颜色影响（或者甚至可能反射一个物体特定的镜面高光颜色）
      float shininess; 影响镜面高光的散射/半径
  }; 
  
  struct Light {
      vec3 position; 光的位置
  
      vec3 ambient;
      vec3 diffuse;
      vec3 specular;
  };
  
  
  uniform Material material;
  uniform Light light;
  
  void main()
  {
      vec3 ambient = light.ambient * material.ambient * lightColor;
  
      vec3 norm = normalize(Normal);
      vec3 lightDir = normalize(light.position - worldPos);
      vec3 diffuse = light.diffuse * lightColor * (max(dot(norm, lightDir), 0.0) * material.diffuse);
  
      vec3 viewDir = normalize(viewPos - worldPos);
      vec3 reflectDir = reflect(-lightDir, norm);
      vec3 spec = light.specular * lightColor * (pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * material.specular);
  
      vec3 result = ambient + diffuse + spec;
  
      vec4 tex = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
      FragColor = vec4(result, 1.0);
  }
  ```

* 光照贴图

  * 漫反射贴图

    一个表现了物体所有的漫反射颜色的纹理图像

    通过贴图来决定物体的颜色

    `sampler2D`是所谓的不透明类型

    ```C++
    struct Material {
        sampler2D diffuse;
        vec3      specular;
        float     shininess;
    }; 
    ```

  * 镜面光贴图

* 投光物

  * 平行光

    光的方向与光源的位置没有关系 定向光

    **定义一个光线方向向量而不是位置向量来模拟一个定向光**

    根据w分量来判断是光的方向向量还是位置向量

  * 点光源

    衰减

    随着光线传播距离的增长逐渐削减光的强度通常叫做衰减(Attenuation)
    $$
    \begin{equation} F_{att} = \frac{1.0}{K_c + K_l * d + K_q * d^2} \end{equation}
    $$

  * 聚光

    **只朝一个特定方向而不是所有方向照射光线**

    聚光是用一个**世界空间位置**、一个**方向**和一个**切光角**(Cutoff Angle)来表示

    - `LightDir`：从片段指向光源的向量。
    - `SpotDir`：聚光所指向的方向。
    - `Phi`ϕϕ：指定了聚光半径的切光角。落在这个角度之外的物体都不会被这个聚光所照亮。
    - `Theta`θθ：LightDir向量和SpotDir向量之间的夹角。在聚光内部的话θθ值应该比ϕϕ值小。

  * 手电筒

  * 平滑/软边缘化

    需要一个外圆锥，来让光从内圆锥逐渐减暗，直到外圆锥的边界
    $$
    \begin{equation} I = \frac{\theta - \gamma}{\epsilon} \end{equation}
    $$
    ϵ(Epsilon)是内（ϕ）和外圆锥（γ）之间的余弦值差（ϵ=ϕ−γ）。最终的II值就是在当前片段聚光的强度。

    对环境光没有影响

    ```
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    
    ...
    // 将不对环境光做出影响，让它总是能有一点光
    diffuse  *= intensity;
    specular *= intensity;
    ```

    clamp函数，它把第一个参数约束(Clamp)在了0.0到1.0之间。这保证强度值不会在[0, 1]区间之外

* 多光源

  