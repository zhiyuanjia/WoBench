#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
// SPDX-License-Identifier: GPL-2.0
#include <jvmti.h>
#include <jvmticmlr.h>

int main(void)
{
	JavaVM			jvm	__attribute__((unused));
	jvmtiEventCallbacks	cb	__attribute__((unused));
	jvmtiCapabilities	caps	__attribute__((unused));
	jvmtiJlocationFormat	format	__attribute__((unused));
	jvmtiEnv		jvmti	__attribute__((unused));

	return 0;
}
