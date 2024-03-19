


# 효율적인 모션 블러 효과 구현 (2022.01 - 2023.02)

<br>


## 1) accumulation 방식을 통한 모션 블러 효과 구현 (2022.01 - 2022.02)

<br>

한 프레임에서 발생한 물체의 움직임을 여러 이미지로 나눠 같은 위치의 픽셀 색상을 평균, 하나의 모션 블러 효과가 적용된 이미지를 생성하는 방법입니다.


<p align="Left">
  <img src="https://github.com/limbsoo/limbsoo.github.io/assets/96706760/256ac94a-3865-41ab-a857-bf9e9a16f7c0" align="center" width="100%">
</p>

<br>
<br>

## 2) 모션 벡터 공유를 통한 모션 블러 효과 구현 (2022.02 - 2022.03)

<br>

물체의 이동 정보를 가진 모션 벡터를 픽셀에 저장,
<p align="Left">
  <img src="https://github.com/limbsoo/limbsoo.github.io/assets/96706760/0685941d-3ecc-4347-b7a2-bd0f5d85d5ea" align="center" width="80%">
</p>

<br>

해당 픽셀을 지나는 모션 벡터 역방향으로 일정 구간 별 샘플하여 색상을 평균, 하나의 모션 블러 효과가 적용된 이미지를 생성합니다.

<p align="Left">
  <img src="https://github.com/limbsoo/limbsoo.github.io/assets/96706760/b6c1d0c5-1f42-4fed-829f-a8261cde682c" align="center" width="100%">
</p>

<br>

이는 기존 accumulation 방식을 따르면서도, 렌더링에 사용하는 이미지를 줄여, 높은 성능을 가진 현실적 모션 블러 효과를 생성합니다.

<br>


기능
- 모션 벡터 길이에 따른 탐색 구역을 생성, 지정된 구역 안에서 모션 벡터 탐색을 진행하여 탐색 범위 축소
- 탐색 구역 안의 랜덤 픽셀을 지정, 해당 픽셀의 모션 벡터를 역추적하여 현재 픽셀을 지나는 모션 벡터를 가진 픽셀을 탐색하여 탐색 횟수 감소
- 랜덤 픽셀 샘플 과정에서 발생하는 아티팩트를 제거하기 위한 디노이징 과정 추가


## 3) 다층 모션 벡터 활용 모션 블러 효과 구현 (2022.04 - 2022.08)

<br>

문제점
	모션 벡터 공유를 통한 모션 블러 효과 구현 시, 해당 픽셀을 지나는 모션 벡터가 해당 픽셀의 모션 벡터와 같은 크기일 때 해당 픽셀 색상을 샘플합니다. 이로 인해 모션 벡터가 0인 배경을 샘플할 수 없고, 여러 물체의 모션 블러 생성 위치가 겹칠 경우, 맨 앞에 위치한 물체의 모션 블러 효과만 생성되는 문제가 발생합니다.


<p align="Left">
  <img src="https://github.com/limbsoo/limbsoo.github.io/assets/96706760/27b88244-fef8-40c2-8c6a-715a083a1251" align="center" width="50%">
</p>

<br>

이를 해결하기 위해 HLSL의 **discard**를 사용, depth 별 이미지들을 생성합니다.

```
if (depth <= PreviousLayerDepthMap[current pixel])
   discard
else
   rendering
```

<br>

이를 통해 탐색에 실패한 경우, 다음 depth 이미지로 이동, 해당 픽셀의 모션 벡터와 비교하여 배경 또는 다른 물체의 색상을 샘플합니다.

<p align="Left">
  <img src="https://github.com/limbsoo/limbsoo.github.io/assets/96706760/ac53a233-08b0-485f-b327-d9b06a6cd140" align="center" width="64%">
</p>

<br>

기능
- HLSL의 **discard**를 활용한 depth 별 이미지들을 생성
- 현재 픽셀의 모션 벡터의 역방향으로 이동, 현재 픽셀과 같은 모션 벡터를 가진 픽셀 샘플하여 탐색 횟수 감소
- 샘플 성공률에 따른 가중치를 통해 디노이징 성능 향상



<br>
<br>



## 4) Stencil routing을 통한 다층 모션 벡터 활용 모션 블러 효과 구현 (2022.08 - 2023.01)

문제점
	**다층 모션 벡터 활용 모션 블러**는 **모션 벡터 공유를 통한 모션 블러**의 문제를 해결하지만 사용하는 이미지의 개수를 늘리면서 성능의 하락을 발생 시킵니다. 

<br>

따라서 기존 이미지 사용에 사용하는 buffer 대신 stencil buffer를 사용하는 **stencil routing** 방법을 통해 멀티 샘플에 사용되는 버퍼를 사용, 같은 픽셀 위치를 depth별로 샘플하여 depth 별 이미지를 생성합니다.

```
if(StencilRef == stencilBuffer)
   rendering & stencil decrease
else
    stencil decrease
```

<br>

Depth Peeling by Stencil Routed Rendering

- Turn off Depth Test & Set multiple render target(renderTarget은 각각의 Stencil buffer와 연결)

- Stencil buffer를 (StencilRef ~ StencilRef + renderTarget - 1)로 초기화

- polygon이 픽셀을 cover할 때 마다 stencilRef와 똑같은 stencil 값을 가지는 renderTarget에만 색상을 저장하고 모든 renderTarget의 stencil값 1 감소

- 여러 개의 polygon이 동일한 픽셀을 cover하면 renderTarget 만큼 polygon의 색상이 renderTarget에 저장된다.
 
<br>


기능
- Stencil Routed Rendering을 통한 depth 별 이미지 생성
- pack, unpack을 활용, rendering에 사용하는 이미지 개수 감소


<br>
<br>

<br>



# DirectX 11 환경 렌더링 (2021.10 - 2021.12)


DXUT 라이브러리를 활용, DirectX11 framework와 rendering pipeline을 구현하고 Lighting, Shadow mapping, Environment Mapping 등 다양한 기법들을 구현하였다.

<br>

구현
- Framework
- Rendering pipeline
- Texture mapping
- Lighting
- Render to texture
- Shadow mapping
- Environment mapping







