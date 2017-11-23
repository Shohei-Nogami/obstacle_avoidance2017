#include"img_prc_cls.h"
//memory
	void ImageProcesser::reserve_vectors(void){
		pts.reserve(point_size);   //特徴点
		npts.reserve(point_size);  //移動後の特徴点
		sts.reserve(point_size);
		ers.reserve(point_size);
//-----特徴点抽出用変数-----
		keypoints.reserve(point_size);
//Provisional vector z
		pz.reserve(point_size);
		points.reserve(point_size);
		newpoints.reserve(point_size);
		z.reserve(point_size);
		nz.reserve(point_size);
		for(int i=0;i<cnh;i++){
			for(int j=0;j<cnw;j++){
				cp[i][j].reserve(clp_point_size);
				cpt[i][j].reserve(clp_point_size);
				cnpt[i][j].reserve(clp_point_size);
				cz[i][j].reserve(clp_point_size);
			}
		}
		jnpts.reserve(point_size);
		jnewpoints.reserve(point_size);
		mv_area.reserve(cnh*cnw);
		opt.reserve(cnh*cnw);

		pk.reserve(point_size);
		npk.reserve(point_size);
		xk_hat.reserve(point_size);
		nxk_hat.reserve(point_size);
		x_hat.reserve(point_size);
		mk.reserve(point_size);
		nmk.reserve(point_size);
//		mk_once_flag.reserve(point_size);
//		nmk_once_flag.reserve(point_size);
	}
	void ImageProcesser::clear_vectors(void){
//		pts.clear();   //特徴点
		npts.clear();  //移動後の特徴点
		sts.clear();
		ers.clear();
		nmk.clear();
		npk.clear();
//		nmk_once_flag.clear();
		
//-----特徴点抽出用変数-----
		keypoints.clear();
		for(int i=0;i<cnh;i++){
			for(int j=0;j<cnw;j++){
				cp[i][j].clear();
			}
		}	
		jnpts.clear();
	}
	void ImageProcesser::renew_vectors(void){
		if(pts.empty())
			return ;
		pts.clear();
		pz.clear();
		pts.insert(pts.end(),newpoints.begin(),newpoints.end());
		pz.insert(pz.end(),nz.begin(),nz.end());
		mk.clear();
		mk.insert(mk.end(),nmk.begin(),nmk.end());
		pk.clear();
		pk.insert(pk.end(),npk.begin(),npk.end());
		xk_hat.clear();
		xk_hat.insert(xk_hat.end(),nxk_hat.begin(),nxk_hat.end());
//		mk_once_flag.clear();
//		mk_once_flag.insert(mk_once_flag.end(),nmk_once_flag.begin(),nmk_once_flag.end());
		
	}
	void ImageProcesser::clear_dtctvectors(void){
		points.clear();
		newpoints.clear();
		jnewpoints.clear();
		z.clear();	
		nz.clear();
		for(int i=0;i<cnh;i++){
			for(int j=0;j<cnw;j++){
				cpt[i][j].clear();
				cnpt[i][j].clear();
				cz[i][j].clear();
			}
		}
		mv_area.clear();
		opt.clear();
		
		pxk_hat.clear();
		nxk_hat.clear();
		x_hat.clear();
	}


