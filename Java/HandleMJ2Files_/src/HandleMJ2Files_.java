<<<<<<< HEAD
import ij.*;
import ij.gui.GenericDialog;
import ij.io.*;
import java.io.*;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.prefs.Preferences;

import com.sun.jna.*;
import com.sun.jna.ptr.*;
import ij.plugin.PlugIn;
import libs.IMJ2_Funcs;

public class HandleMJ2Files_  implements PlugIn {
	public void run(String path) 
	{
		Path p1 = Paths.get(path);
		String name =  p1.getFileName().toString();
		String original_file_name=name;
		name = name.toLowerCase();		
		String[] Pxl_Format={"DEFAULT","YUV420P","YUYV422","RGB24","BGR24","YUV422P","YUV444P","YUV410P","YUV411P","GRAY8","MONOWHITE","MONOBLACK","PAL8","YUVJ420P","YUVJ422P","YUVJ444P","UYVY422","UYYVYY411","BGR8","BGR4","BGR4_BYTE","RGB8","RGB4","RGB4_BYTE","NV12","NV21","ARGB","RGBA","ABGR","BGRA","GRAY16BE","GRAY16LE","YUV440P","YUVJ440P","YUVA420P","RGB48BE","RGB48LE","RGB565BE","RGB565LE","RGB555BE","RGB555LE","BGR565BE","BGR565LE","BGR555BE","BGR555LE","VAAPI","YUV420P16LE","YUV420P16BE","YUV422P16LE","YUV422P16BE","YUV444P16LE","YUV444P16BE","VDPAU_MPEG4","DXVA2_VLD","RGB444LE","RGB444BE","BGR444LE","BGR444BE","YA8","Y400A","GRAY8A","BGR48BE","BGR48LE","YUV420P9BE","YUV420P9LE","YUV420P10BE","YUV420P10LE","YUV422P10BE","YUV422P10LE","YUV444P9BE","YUV444P9LE","YUV444P10BE","YUV444P10LE","YUV422P9BE","YUV422P9LE","VDA_VLD","GBRP","GBRP9BE","GBRP9LE","GBRP10BE","GBRP10LE","GBRP16BE","GBRP16LE","YUVA422P","YUVA444P","YUVA420P9BE","YUVA420P9LE","YUVA422P9BE","YUVA422P9LE","YUVA444P9BE","YUVA444P9LE","YUVA420P10BE","YUVA420P10LE","YUVA422P10BE","YUVA422P10LE","YUVA444P10BE","YUVA444P10LE","YUVA420P16BE","YUVA420P16LE","YUVA422P16BE","YUVA422P16LE","YUVA444P16BE","YUVA444P16LE","VDPAU","XYZ12LE","XYZ12BE","NV16","NV20LE","NV20BE","RGBA64BE","RGBA64LE","BGRA64BE","BGRA64LE","YVYU422","VDA","YA16BE","YA16LE","GBRAP","GBRAP16BE","GBRAP16LE","QSV","MMAL","D3D11VA_VLD","CUDA","0RGB=0x123+4","RGB0","0BGR","BGR0","YUV420P12BE","YUV420P12LE","YUV420P14BE","YUV420P14LE","YUV422P12BE","YUV422P12LE","YUV422P14BE","YUV422P14LE","YUV444P12BE","YUV444P12LE","YUV444P14BE","YUV444P14LE","GBRP12BE","GBRP12LE","GBRP14BE","GBRP14LE","YUVJ411P","BAYER_BGGR8","BAYER_RGGB8","BAYER_GBRG8","BAYER_GRBG8","BAYER_BGGR16LE","BAYER_BGGR16BE","BAYER_RGGB16LE","BAYER_RGGB16BE","BAYER_GBRG16LE","BAYER_GBRG16BE","BAYER_GRBG16LE","BAYER_GRBG16BE","YUV440P10LE","YUV440P10BE","YUV440P12LE","YUV440P12BE","AYUV64LE","AYUV64BE","VIDEOTOOLBOX","P010LE","P010BE","GBRAP12BE","GBRAP12LE","NB"};
		
		int jvm_bit = Integer.parseInt(System.getProperty("sun.arch.data.model"));
		//if(jvm_bit==64)
		//	IJ.showMessage("MJ2 File Decoder", "MJ2 File Decoder plug-in by Lagnajeet Pradhan.\nThis is a Platform dependent Plugin which uses ffmpeg for the decoding.\n \nVersion 1.0 (64 Bit Windows)\nffmpeg version 20160428-git-78baa45\nCompiler GCC V 5.3.0 (x86_64-posix-sjlj-rev0)\n \nContact lagnajeet@gmail.com");
		//else
		//	IJ.showMessage("MJ2 File Decoder", "MJ2 File Decoder plug-in by Lagnajeet Pradhan.\nThis is a Platform dependent Plugin which uses ffmpeg for the decoding.\n \nVersion 1.0 (32 Bit Windows)\nffmpeg version 20160510-git-9486de5\nCompiler GCC V 5.3.0 (i686-win32-dwarf-rev0)\n \nContact lagnajeet@gmail.com");
		Preferences prefs = Preferences.userNodeForPackage(this.getClass());
		int number_of_threads=prefs.getInt("number_of_threads", 6);
		int pixel_format=prefs.getInt("pixel_format", 0);		
		if (name.endsWith(".mj2")) 
		{
			//IJ.error("Hello world!");
			//final long startTime = System.currentTimeMillis();
			IMJ2_Funcs mj2_Funcs;
			if(jvm_bit==32)
				mj2_Funcs =  (IMJ2_Funcs)Native.loadLibrary("mj2_lib", IMJ2_Funcs.class);
			else
				mj2_Funcs =  (IMJ2_Funcs)Native.loadLibrary("mj2_lib_64", IMJ2_Funcs.class);
			mj2_Funcs.setup_decoder(Pxl_Format[pixel_format],number_of_threads);
			Pointer p=null;
			int frame_count=0;
			DoubleByReference nb_frames = new DoubleByReference();
			DoubleByReference img_height = new DoubleByReference();
			DoubleByReference img_width = new DoubleByReference();
			mj2_Funcs.open_input_file(path,nb_frames,img_height,img_width);
			int total_frames=(int)nb_frames.getValue();
			//System.out.println(img_height.getValue()+","+img_width.getValue()+","+nb_frames.getValue());
			mj2_Funcs.extract_frames();
			while(frame_count<total_frames)
			{
				frame_count=mj2_Funcs.number_of_frames_decoded();
				//System.out.println(frame_count);
				if(frame_count==-1)
					break;
				IJ.showStatus("Frame "+Integer.toString(frame_count+1)+" of "+Integer.toString((int)nb_frames.getValue()) + " decoded.");
				IJ.showProgress(frame_count,total_frames);
			}
			
			if(frame_count==-1)
			{
				IJ.showStatus("Error Decoding file");
				IJ.showProgress(total_frames,total_frames);						
				IJ.error("Error Opening File. Trying a different setting might be helpful'");		
				return;
			}
			int w=(int)img_width.getValue();
			int h=(int)img_height.getValue();
			
			ImageStack stack = new ImageStack(w, h);
			frame_count=0;
			while(frame_count<total_frames)
			{
				DoubleByReference outout_size = new DoubleByReference();
				p=mj2_Funcs.getFramesAsTiff(frame_count,outout_size);
				//System.out.println(frame_count);
				byte[] byteData = p.getByteArray(0, (int) outout_size.getValue());
				TiffDecoder td = new TiffDecoder(new ByteArrayInputStream(byteData), original_file_name); 
				FileInfo[] info = null;
				try {
					info = td.getTiffInfo();
				} catch (IOException e) {
					e.printStackTrace();
				} 
				ImagePlus imp = new ImagePlus(); 
				FileOpener fo = new FileOpener(info[0]); 
				imp = fo.open(false);	
				//mj2_Funcs.free_frame(frame_count);
				frame_count++;
				IJ.showStatus("Opening Frame "+Integer.toString(frame_count)+" of "+Integer.toString((int)nb_frames.getValue()));
				IJ.showProgress(frame_count,total_frames);
				stack.addSlice("", imp.getProcessor());		
			}
			ImagePlus image = new ImagePlus(original_file_name, stack);
			mj2_Funcs.free_memory();
			//NativeLibrary.dispose();			
			//final long endTime = System.currentTimeMillis();
			//System.out.println("Total execution time: " + (endTime - startTime) );
			//System.out.println(System.getProperty("sun.arch.data.model")) ;
			mj2_Funcs=null;
			image.show();
		}
		else
		{
			String html ="<html><title>About MJ2 Decoder Plugin</title>"
				     +"<h1>MJ2 File Decoder plug-in by Lagnajeet Pradhan.</h1>"
				     +"<font size=4>"
			 		+"This is a Platform dependent Plugin which uses ffmpeg for the decoding.<br><br>";
			if(jvm_bit==64)
				html = html  +"Version 1.1 (64 Bit Windows)<br>ffmpeg version 20160428-git-78baa45<br>Compiler GCC V 5.3.0 (x86_64-posix-sjlj-rev0)<br><br>";
			else
				html = html +"Version 1.1 (32 Bit Windows)<br>ffmpeg version 20160510-git-9486de5<br>Compiler GCC V 5.3.0 (i686-win32-dwarf-rev0)<br><br>";
					     					
			html=html+"Contact: <font color='blue'><a href=mailto:lagnajeet@gmail.com>lagnajeet@gmail.com</a></font></font>";
			GenericDialog gd = new GenericDialog("Decoder settings");
			gd.addSlider("Number of Threads:",1,20,number_of_threads);
			gd.addChoice("Force Pixel Format:",Pxl_Format,Pxl_Format[pixel_format]);
			gd.addHelp(html);
			gd.setHelpLabel("About");
			
			// show the dialog and quit, if the user clicks "cancel"
			gd.showDialog();
			if (gd.wasCanceled()) {
				//IJ.error("PlugIn canceled!");
				return;
			}
			if (gd.wasOKed()) {
				prefs.putInt("number_of_threads", (int) gd.getNextNumber());
				prefs.putInt("pixel_format", (int) gd.getNextChoiceIndex());
				return;
			}
			return;			
		}
		//	return null;
	}
=======
import ij.*;
import ij.gui.GenericDialog;
import ij.io.*;
import java.io.*;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.prefs.Preferences;

import com.sun.jna.*;
import com.sun.jna.ptr.*;
import ij.plugin.PlugIn;
import libs.IMJ2_Funcs;

public class HandleMJ2Files_  implements PlugIn {
	public void run(String path) 
	{
		Path p1 = Paths.get(path);
		String name =  p1.getFileName().toString();
		String original_file_name=name;
		name = name.toLowerCase();		
		String[] Pxl_Format={"DEFAULT","YUV420P","YUYV422","RGB24","BGR24","YUV422P","YUV444P","YUV410P","YUV411P","GRAY8","MONOWHITE","MONOBLACK","PAL8","YUVJ420P","YUVJ422P","YUVJ444P","UYVY422","UYYVYY411","BGR8","BGR4","BGR4_BYTE","RGB8","RGB4","RGB4_BYTE","NV12","NV21","ARGB","RGBA","ABGR","BGRA","GRAY16BE","GRAY16LE","YUV440P","YUVJ440P","YUVA420P","RGB48BE","RGB48LE","RGB565BE","RGB565LE","RGB555BE","RGB555LE","BGR565BE","BGR565LE","BGR555BE","BGR555LE","VAAPI","YUV420P16LE","YUV420P16BE","YUV422P16LE","YUV422P16BE","YUV444P16LE","YUV444P16BE","VDPAU_MPEG4","DXVA2_VLD","RGB444LE","RGB444BE","BGR444LE","BGR444BE","YA8","Y400A","GRAY8A","BGR48BE","BGR48LE","YUV420P9BE","YUV420P9LE","YUV420P10BE","YUV420P10LE","YUV422P10BE","YUV422P10LE","YUV444P9BE","YUV444P9LE","YUV444P10BE","YUV444P10LE","YUV422P9BE","YUV422P9LE","VDA_VLD","GBRP","GBRP9BE","GBRP9LE","GBRP10BE","GBRP10LE","GBRP16BE","GBRP16LE","YUVA422P","YUVA444P","YUVA420P9BE","YUVA420P9LE","YUVA422P9BE","YUVA422P9LE","YUVA444P9BE","YUVA444P9LE","YUVA420P10BE","YUVA420P10LE","YUVA422P10BE","YUVA422P10LE","YUVA444P10BE","YUVA444P10LE","YUVA420P16BE","YUVA420P16LE","YUVA422P16BE","YUVA422P16LE","YUVA444P16BE","YUVA444P16LE","VDPAU","XYZ12LE","XYZ12BE","NV16","NV20LE","NV20BE","RGBA64BE","RGBA64LE","BGRA64BE","BGRA64LE","YVYU422","VDA","YA16BE","YA16LE","GBRAP","GBRAP16BE","GBRAP16LE","QSV","MMAL","D3D11VA_VLD","CUDA","0RGB=0x123+4","RGB0","0BGR","BGR0","YUV420P12BE","YUV420P12LE","YUV420P14BE","YUV420P14LE","YUV422P12BE","YUV422P12LE","YUV422P14BE","YUV422P14LE","YUV444P12BE","YUV444P12LE","YUV444P14BE","YUV444P14LE","GBRP12BE","GBRP12LE","GBRP14BE","GBRP14LE","YUVJ411P","BAYER_BGGR8","BAYER_RGGB8","BAYER_GBRG8","BAYER_GRBG8","BAYER_BGGR16LE","BAYER_BGGR16BE","BAYER_RGGB16LE","BAYER_RGGB16BE","BAYER_GBRG16LE","BAYER_GBRG16BE","BAYER_GRBG16LE","BAYER_GRBG16BE","YUV440P10LE","YUV440P10BE","YUV440P12LE","YUV440P12BE","AYUV64LE","AYUV64BE","VIDEOTOOLBOX","P010LE","P010BE","GBRAP12BE","GBRAP12LE","NB"};
		
		int jvm_bit = Integer.parseInt(System.getProperty("sun.arch.data.model"));
		//if(jvm_bit==64)
		//	IJ.showMessage("MJ2 File Decoder", "MJ2 File Decoder plug-in by Lagnajeet Pradhan.\nThis is a Platform dependent Plugin which uses ffmpeg for the decoding.\n \nVersion 1.0 (64 Bit Windows)\nffmpeg version 20160428-git-78baa45\nCompiler GCC V 5.3.0 (x86_64-posix-sjlj-rev0)\n \nContact lagnajeet@gmail.com");
		//else
		//	IJ.showMessage("MJ2 File Decoder", "MJ2 File Decoder plug-in by Lagnajeet Pradhan.\nThis is a Platform dependent Plugin which uses ffmpeg for the decoding.\n \nVersion 1.0 (32 Bit Windows)\nffmpeg version 20160510-git-9486de5\nCompiler GCC V 5.3.0 (i686-win32-dwarf-rev0)\n \nContact lagnajeet@gmail.com");
		Preferences prefs = Preferences.userNodeForPackage(this.getClass());
		int number_of_threads=prefs.getInt("number_of_threads", 6);
		int pixel_format=prefs.getInt("pixel_format", 0);		
		if (name.endsWith(".mj2")) 
		{
			//IJ.error("Hello world!");
			//final long startTime = System.currentTimeMillis();
			IMJ2_Funcs mj2_Funcs;
			if(jvm_bit==32)
				mj2_Funcs =  (IMJ2_Funcs)Native.loadLibrary("mj2_lib", IMJ2_Funcs.class);
			else
				mj2_Funcs =  (IMJ2_Funcs)Native.loadLibrary("mj2_lib_64", IMJ2_Funcs.class);
			mj2_Funcs.setup_decoder(Pxl_Format[pixel_format],number_of_threads);
			Pointer p=null;
			int frame_count=0;
			DoubleByReference nb_frames = new DoubleByReference();
			DoubleByReference img_height = new DoubleByReference();
			DoubleByReference img_width = new DoubleByReference();
			mj2_Funcs.open_input_file(path,nb_frames,img_height,img_width);
			int total_frames=(int)nb_frames.getValue();
			//System.out.println(img_height.getValue()+","+img_width.getValue()+","+nb_frames.getValue());
			mj2_Funcs.extract_frames();
			while(frame_count<total_frames)
			{
				frame_count=mj2_Funcs.number_of_frames_decoded();
				//System.out.println(frame_count);
				if(frame_count==-1)
					break;
				IJ.showStatus("Frame "+Integer.toString(frame_count+1)+" of "+Integer.toString((int)nb_frames.getValue()) + " decoded.");
				IJ.showProgress(frame_count,total_frames);
			}
			
			if(frame_count==-1)
			{
				IJ.showStatus("Error Decoding file");
				IJ.showProgress(total_frames,total_frames);						
				IJ.error("Error Opening File. Trying a different setting might be helpful'");		
				return;
			}
			int w=(int)img_width.getValue();
			int h=(int)img_height.getValue();
			
			ImageStack stack = new ImageStack(w, h);
			frame_count=0;
			while(frame_count<total_frames)
			{
				DoubleByReference outout_size = new DoubleByReference();
				p=mj2_Funcs.getFramesAsTiff(frame_count,outout_size);
				//System.out.println(frame_count);
				byte[] byteData = p.getByteArray(0, (int) outout_size.getValue());
				TiffDecoder td = new TiffDecoder(new ByteArrayInputStream(byteData), original_file_name); 
				FileInfo[] info = null;
				try {
					info = td.getTiffInfo();
				} catch (IOException e) {
					e.printStackTrace();
				} 
				ImagePlus imp = new ImagePlus(); 
				FileOpener fo = new FileOpener(info[0]); 
				imp = fo.open(false);	
				//mj2_Funcs.free_frame(frame_count);
				frame_count++;
				IJ.showStatus("Opening Frame "+Integer.toString(frame_count)+" of "+Integer.toString((int)nb_frames.getValue()));
				IJ.showProgress(frame_count,total_frames);
				stack.addSlice("", imp.getProcessor());		
			}
			ImagePlus image = new ImagePlus(original_file_name, stack);
			mj2_Funcs.free_memory();
			//NativeLibrary.dispose();			
			//final long endTime = System.currentTimeMillis();
			//System.out.println("Total execution time: " + (endTime - startTime) );
			//System.out.println(System.getProperty("sun.arch.data.model")) ;
			mj2_Funcs=null;
			image.show();
		}
		else
		{
			String html ="<html><title>About MJ2 Decoder Plugin</title>"
				     +"<h1>MJ2 File Decoder plug-in by Lagnajeet Pradhan.</h1>"
				     +"<font size=4>"
			 		+"This is a Platform dependent Plugin which uses ffmpeg for the decoding.<br><br>";
			if(jvm_bit==64)
				html = html  +"Version 1.1 (64 Bit Windows)<br>ffmpeg version 20160428-git-78baa45<br>Compiler GCC V 5.3.0 (x86_64-posix-sjlj-rev0)<br><br>";
			else
				html = html +"Version 1.1 (32 Bit Windows)<br>ffmpeg version 20160510-git-9486de5<br>Compiler GCC V 5.3.0 (i686-win32-dwarf-rev0)<br><br>";
					     					
			html=html+"Contact: <font color='blue'><a href=mailto:lagnajeet@gmail.com>lagnajeet@gmail.com</a></font></font>";
			GenericDialog gd = new GenericDialog("Decoder settings");
			gd.addSlider("Number of Threads:",1,20,number_of_threads);
			gd.addChoice("Force Pixel Format:",Pxl_Format,Pxl_Format[pixel_format]);
			gd.addHelp(html);
			gd.setHelpLabel("About");
			
			// show the dialog and quit, if the user clicks "cancel"
			gd.showDialog();
			if (gd.wasCanceled()) {
				//IJ.error("PlugIn canceled!");
				return;
			}
			if (gd.wasOKed()) {
				prefs.putInt("number_of_threads", (int) gd.getNextNumber());
				prefs.putInt("pixel_format", (int) gd.getNextChoiceIndex());
				return;
			}
			return;			
		}
		//	return null;
	}
>>>>>>> origin/master
}