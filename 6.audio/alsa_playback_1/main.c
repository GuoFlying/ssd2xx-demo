#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	long loops;
	int rc, j = 0;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val, val2;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;
	FILE *fp;
	if (argc < 2)
	{
		printf("please input a wav file!\n");
		printf("eg: %s ./res/48K_16bit_STERO_30s.wav\n", argv[0]);
		return -1;
	}
	if ((fp = fopen(argv[1], "r")) < 0){
		printf("open sound.pcm fial\n");
	}
	//if(fseek(fp,0,SEEK_SET) < 0)
	// printf("put fp start to first error\n ");

	/* 打开PCM PLAYBACK 驱动 */
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0)
	{
		fprintf(stderr, "unable to open pcm device: %s/n", snd_strerror(rc));
		exit(1);
	}
	/* 分配硬件参数对象 */
	snd_pcm_hw_params_alloca(&params);
	/*设置默认值. */
	snd_pcm_hw_params_any(handle, params);
	/* 设置需要的硬件参数. */
	/* 交错模式 */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	/* 16bit 小端模式 */
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
	/* 双通道 */
	snd_pcm_hw_params_set_channels(handle, params, 2);
	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
	/* Set period size to 32 frames. */
	frames = val * 2 / 8;												   //设置的值没有反应
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir); //
	printf("frames is %d\n", (int)frames);
	/* 将参数写入驱动程序 */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0)
	{
		fprintf(stderr, "unable to set hw parameters: %s/n", snd_strerror(rc));
		exit(1);
	}
	/* 使用足够大的缓冲区来容纳一个周期 */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *)malloc(size);

	/* 我们想要循环5秒 */
	snd_pcm_hw_params_get_period_time(params, &val, &dir);

	/* 5秒(微秒)除以*周期时间 */
	loops = 10000000 / val;
	//fseek(fp,44,SEEK_SET); //play wav format subtract 44 bytes
	while (1)
	{
		loops--;
		rc = fread(buffer, 1, size, fp);
		//rc = read(0,buffer,size);

		//printf("%d\n",j++);
		if (rc == 0)
		{
			fprintf(stderr, "end of file on input\n");
			break;
		}
		else if (rc != size)
		{
			fprintf(stderr, "short read: read %d bytes\n", rc);
		}
		else
		{
			printf("fread to buffer success\n");
		}
		rc = snd_pcm_writei(handle, buffer, frames);
		if (rc == -EPIPE)
		{
			/* EPIPE means underrun */
			fprintf(stderr, "underrun occurred\n");

			snd_pcm_prepare(handle);
		}
		else if (rc < 0)
		{
			fprintf(stderr, "error from writei: %s\n",
					snd_strerror(rc));
		}
		else if (rc != (int)frames)
		{
			fprintf(stderr, "short write, write %d frames\n", rc);
		}
		else
		{
			printf("snd_pcm_writei success.\n");
		}
		usleep(5 * 1000);
	}
	/******************打印参数*********************/
	snd_pcm_hw_params_get_channels(params, &val);
	printf("channels = %d\n", val);
	snd_pcm_hw_params_get_rate(params, &val, &dir);
	printf("rate = %d bps\n", val);
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	printf("period time = %d us\n", val);

	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf("period size = %d frames\n", (int)frames);
	snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
	printf("buffer time = %d us\n", val);
	snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *)&val);
	printf("buffer size = %d frames\n", val);
	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);
	snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
	printf("exact rate = %d/%d bps\n", val, val2);
	val = snd_pcm_hw_params_get_sbits(params);
	printf("significant bits = %d\n", val);

	//snd_pcm_hw_params_get_tick_time(params,  &val, &dir);
	printf("tick time = %d us\n", val);
	val = snd_pcm_hw_params_is_batch(params);
	printf("is batch = %d\n", val);
	val = snd_pcm_hw_params_is_block_transfer(params);
	printf("is block transfer = %d\n", val);
	val = snd_pcm_hw_params_is_double(params);
	printf("is double = %d\n", val);

	val = snd_pcm_hw_params_is_half_duplex(params);
	printf("is half duplex = %d\n", val);
	val = snd_pcm_hw_params_is_joint_duplex(params);
	printf("is joint duplex = %d\n", val);
	val = snd_pcm_hw_params_can_overrange(params);
	printf("can overrange = %d\n", val);
	val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
	printf("can mmap = %d\n", val);
	val = snd_pcm_hw_params_can_pause(params);
	printf("can pause = %d\n", val);
	val = snd_pcm_hw_params_can_resume(params);
	printf("can resume = %d\n", val);
	val = snd_pcm_hw_params_can_sync_start(params);
	printf("can sync start = %d\n", val);
	/*******************************************************************/
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);

	fclose(fp);

	return 0;
}