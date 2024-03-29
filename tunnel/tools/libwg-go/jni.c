/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright © 2017-2021 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 */

#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct go_string { const char *str; long n; };
extern int wgTurnOn(struct go_string ifname, int tun_fd, struct go_string settings, struct go_string socket_type);
extern void wgTurnOff(int handle);
extern int wgSetNetworkAvailable(int handle, int available);
extern int wgGetState(int handle);
extern int wgGetSocketV4(int handle);
extern int wgGetSocketV6(int handle);
extern char *wgGetConfig(int handle);
extern char *wgVersion();

JavaVM *gJvm = NULL;
jclass gBackendClass = NULL;

jint JNI_OnLoad(JavaVM *pJvm, void *reserved) {
	gJvm = pJvm;
	return JNI_VERSION_1_6;
}

int protectSocket(int fd) {
	bool attached = false;
	JNIEnv *env;
	int status;
	status = (*gJvm)->GetEnv(gJvm, (void**)&env, JNI_VERSION_1_6);
	if (status == JNI_EDETACHED) {
		status = (*gJvm)->AttachCurrentThread(gJvm, &env, NULL);
		if (status < 0)
			return status;
		attached = true;
	} else if (status < 0) {
		return status;
	}

	jmethodID jprotect = (*env)->GetStaticMethodID(env, gBackendClass, "protectSocket", "(I)I");
	if (jprotect == NULL) {
		return -1;
	}

	status = (*env)->CallStaticIntMethod(env, gBackendClass, jprotect, fd);

	// Detach thread to avoid performance impact.
	if (attached) {
		(*gJvm)->DetachCurrentThread(gJvm);
	}

	return status;
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgTurnOn(JNIEnv *env, jclass c, jstring ifname, jint tun_fd, jstring settings, jstring socket_type)
{
	if (gBackendClass == NULL) {
		gBackendClass = (*env)->NewGlobalRef(env, c);
	}

	const char *ifname_str = (*env)->GetStringUTFChars(env, ifname, 0);
	size_t ifname_len = (*env)->GetStringUTFLength(env, ifname);
	const char *settings_str = (*env)->GetStringUTFChars(env, settings, 0);
	size_t settings_len = (*env)->GetStringUTFLength(env, settings);
	const char *socket_type_str = (*env)->GetStringUTFChars(env, socket_type, 0);
	size_t socket_type_len = (*env)->GetStringUTFLength(env, socket_type);
	int ret = wgTurnOn((struct go_string){
		.str = ifname_str,
		.n = ifname_len
	}, tun_fd, (struct go_string){
		.str = settings_str,
		.n = settings_len
	}, (struct go_string){
		.str = socket_type_str,
		.n = socket_type_len
	});
	(*env)->ReleaseStringUTFChars(env, ifname, ifname_str);
	(*env)->ReleaseStringUTFChars(env, settings, settings_str);
	return ret;
}

JNIEXPORT void JNICALL Java_com_wireguard_android_backend_GoBackend_wgTurnOff(JNIEnv *env, jclass c, jint handle)
{
	wgTurnOff(handle);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgSetNetworkAvailable(JNIEnv *env, jclass c, jint handle, jboolean available)
{
	return wgSetNetworkAvailable(handle, available);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetState(JNIEnv *env, jclass c, jint handle)
{
	return wgGetState(handle);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetSocketV4(JNIEnv *env, jclass c, jint handle)
{
	return wgGetSocketV4(handle);
}

JNIEXPORT jint JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetSocketV6(JNIEnv *env, jclass c, jint handle)
{
	return wgGetSocketV6(handle);
}

JNIEXPORT jstring JNICALL Java_com_wireguard_android_backend_GoBackend_wgGetConfig(JNIEnv *env, jclass c, jint handle)
{
	jstring ret;
	char *config = wgGetConfig(handle);
	if (!config)
		return NULL;
	ret = (*env)->NewStringUTF(env, config);
	free(config);
	return ret;
}

JNIEXPORT jstring JNICALL Java_com_wireguard_android_backend_GoBackend_wgVersion(JNIEnv *env, jclass c)
{
	jstring ret;
	char *version = wgVersion();
	if (!version)
		return NULL;
	ret = (*env)->NewStringUTF(env, version);
	free(version);
	return ret;
}
