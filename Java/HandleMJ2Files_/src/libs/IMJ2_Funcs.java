<<<<<<< HEAD
package libs;
import com.sun.jna.Library;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.*;

public interface IMJ2_Funcs extends Library {
	
	int open_input_file(String filename,DoubleByReference nb_frames,DoubleByReference img_height,DoubleByReference img_width);
	Pointer getFramesAsTiff(double frame_number,DoubleByReference output_size);
	Void free_frame(double frame_number);
	//Pointer getFramesAsShort(DoubleByReference linewidth);
	//Void close_input_file();
	Void extract_frames();
	Void free_memory();
	int number_of_frames_decoded(); 
	Void setup_decoder(String format,int num);
=======
package libs;
import com.sun.jna.Library;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.*;

public interface IMJ2_Funcs extends Library {
	
	int open_input_file(String filename,DoubleByReference nb_frames,DoubleByReference img_height,DoubleByReference img_width);
	Pointer getFramesAsTiff(double frame_number,DoubleByReference output_size);
	Void free_frame(double frame_number);
	//Pointer getFramesAsShort(DoubleByReference linewidth);
	//Void close_input_file();
	Void extract_frames();
	Void free_memory();
	int number_of_frames_decoded(); 
	Void setup_decoder(String format,int num);
>>>>>>> origin/master
}